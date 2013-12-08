#include "videoframedumper.h"
#include <QFileInfo>
#include <QDebug>
#include <QImage>

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

VideoFrameDumper::VideoFrameDumper(QUrl path)
{
    QFileInfo fileInfo(path.toLocalFile());
    if (!fileInfo.exists()) {
        qWarning() << "can't open file" << path;
    }

    m_outputPath = fileInfo.canonicalPath();
    m_outputFile = fileInfo.absoluteFilePath().toLocal8Bit();
    m_filename = fileInfo.fileName();
    moveToThread(&m_thread);
    m_thread.start();
}

static void saveFrameToImage(AVFrame *frame, QString path, QString filename, AVCodecContext *video_dec_ctx, int n)
{

    SwsContext* scaleContext = sws_getContext(video_dec_ctx->width, video_dec_ctx->height,
                                              video_dec_ctx->pix_fmt,
                                              video_dec_ctx->width, video_dec_ctx->height,
                                              PIX_FMT_RGB24, SWS_BICUBLIN, NULL, NULL, NULL);

    AVFrame *avFrame = avcodec_alloc_frame();

    int numBytes = avpicture_get_size(PIX_FMT_RGB24, video_dec_ctx->width, video_dec_ctx->height);
    quint8 *frameBuffer = reinterpret_cast<quint8*>(av_malloc(numBytes));

    avpicture_fill((AVPicture*) avFrame, frameBuffer, PIX_FMT_RGB24, video_dec_ctx->width, video_dec_ctx->height);

    sws_scale(scaleContext, frame->data, frame->linesize, 0, video_dec_ctx->height,
              avFrame->data, avFrame->linesize);
    sws_freeContext(scaleContext);

    QImage image(video_dec_ctx->width, video_dec_ctx->height, QImage::Format_RGB888);
    memcpy(image.bits(), &avFrame->data[0][0], image.byteCount());
    QString file = path + "/.vimiframe_" + QString::number(n) + "_" + filename + ".jpg";

    if (!image.save(file))
        qWarning() << "Unable to save file" << file;

    av_free(avFrame);
    av_free(frameBuffer);
}

void VideoFrameDumper::createSnapshots(int num)
{
    AVFormatContext *fmt_ctx = 0;
    av_register_all();


    if (avformat_open_input(&fmt_ctx, m_outputFile.constData(), NULL, NULL) < 0) {
        qWarning() << "Unable to open input" << m_outputFile;
        return;
    }

    if (avformat_find_stream_info(fmt_ctx, 0) < 0) {
        qWarning() << "unable to get stream info";
        return;
    }

    int video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_stream_idx < 0) {
        qWarning() << "unable to open codec context";
        return;
    }
    AVCodec *dec = avcodec_find_decoder(fmt_ctx->streams[video_stream_idx]->codec->codec_id);
    if (!dec) {
        qWarning() << "Unable to find decoder!";
        return;
    }
    if (avcodec_open2(fmt_ctx->streams[video_stream_idx]->codec, dec, 0) < 0) {
        qWarning() << "Unable to initialize codec context";
        return;
    }

    //av_dump_format(fmt_ctx, 0, m_filename.constData(), 0);

    AVFrame *frame = av_frame_alloc();

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;


    int64_t skip_size = (fmt_ctx->duration) / num;
    int i = 0;
    int ret;
    while (true) {
        ret = av_read_frame(fmt_ctx, &pkt);

        if (ret == AVERROR(EAGAIN))
            continue;

        if (ret < 0)
            break;

        if (pkt.stream_index != video_stream_idx) {
            av_free_packet(&pkt);
            continue;
        }

        int got_frame = 0;
        ret = avcodec_decode_video2(fmt_ctx->streams[video_stream_idx]->codec, frame, &got_frame, &pkt);
        if (ret < 0) {
            char errbuf[1024];
            av_strerror(ret, errbuf, 1024);
            qWarning() << "error decoding:" << errbuf << ret;
            break;
        }
        if (got_frame){
            //saveFrameToImage(frame, m_outputPath, m_filename, fmt_ctx->streams[video_stream_idx]->codec, i*skip_size/1000);

            const int width = fmt_ctx->streams[video_stream_idx]->codec->width;
            const int height = fmt_ctx->streams[video_stream_idx]->codec->height;
            SwsContext* scaleContext = sws_getContext(width, height,
                                                      fmt_ctx->streams[video_stream_idx]->codec->pix_fmt,
                                                      width, height,
                                                      PIX_FMT_RGB24, SWS_BICUBLIN, NULL, NULL, NULL);

            AVFrame *avFrame = avcodec_alloc_frame();

            const int numBytes = avpicture_get_size(PIX_FMT_RGB24, width, height);
            quint8 *frameBuffer = reinterpret_cast<quint8*>(av_malloc(numBytes));

            avpicture_fill((AVPicture*) avFrame, frameBuffer, PIX_FMT_RGB24, width, height);

            sws_scale(scaleContext, frame->data, frame->linesize, 0, height, avFrame->data, avFrame->linesize);
            sws_freeContext(scaleContext);

            QImage image(width, height, QImage::Format_RGB888);
            memcpy(image.bits(), &avFrame->data[0][0], image.byteCount());
            QString file = m_outputPath + "/.vimiframe_" + QString::number(i*skip_size/1000) + "_" + m_filename + ".jpg";

            if (!image.save(file))
                qWarning() << "Unable to save file" << file;

            av_free(avFrame);
            av_free(frameBuffer);


            i++;
            if (i>num)
                break;
            emit statusUpdated("Creating snapshots: " + QString::number(i) + '%');

            av_seek_frame(fmt_ctx, -1, skip_size*i, 0);
            avcodec_flush_buffers(fmt_ctx->streams[video_stream_idx]->codec);
        }
        av_free_packet(&pkt);
    }

    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, 1024);
        qWarning() << "error reading frame:" << errbuf << ret;
    }

    avcodec_close(fmt_ctx->streams[video_stream_idx]->codec);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    emit screenshotsCreated(m_outputPath);
    deleteLater();
}
