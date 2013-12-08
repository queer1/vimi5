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
    m_filename = fileInfo.absoluteFilePath().toLocal8Bit();

}

static void saveFrameToImage(AVFrame *frame, QString path, AVCodecContext *video_dec_ctx, int64_t cnt)
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



    /*av_image_copy(video_dst_data, video_dst_linesize,
                  (const uint8_t **)(frame->data), frame->linesize,
                  video_dec_ctx->pix_fmt, video_dec_ctx->width, video_dec_ctx->height);*/

    QImage image(video_dec_ctx->width, video_dec_ctx->height, QImage::Format_RGB888);
    memcpy(image.bits(), &avFrame->data[0][0], image.byteCount());//m_frame->linesize[m_frame->height] * m_frame->height);
    //QString filename = path + '/' + QString::number(frame->pts) + ".jpg";
    QString filename = "/home/test/tmp/" + QString::number(cnt) + ".jpg";
    if (!image.save(filename))
        qWarning() << "Unable to save file" << filename;
    else
        qDebug() << "Savied frame" << filename;


    av_free(avFrame);
    av_free(frameBuffer);

}

void VideoFrameDumper::createSnapshots(int num)
{
    AVFormatContext *fmt_ctx = 0;
    av_register_all();

    int ret=0;
    ret = avformat_open_input(&fmt_ctx, m_filename.constData(), NULL, NULL);
    if (ret < 0) {
        qWarning() << "Unable to open input" << ret << m_filename;
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
    AVDictionary *opts = NULL;
    av_dict_set(&opts, "refcounted_frames", "1", 0);
    if (avcodec_open2(fmt_ctx->streams[video_stream_idx]->codec, dec, &opts) < 0) {
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
    qDebug() << "duration:" << fmt_ctx->duration;
    int i = 0;
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        //qDebug() << "pos:" << pkt.pos;
        AVPacket orig_packet = pkt;
        int got_frame = 0;
        /* decode video frame */

        qDebug() << "pkt.size before:" << pkt.size;
        ret = avcodec_decode_video2(fmt_ctx->streams[video_stream_idx]->codec, frame, &got_frame, &pkt);
        if (ret < 0) {
            char errbuf[1024];
            av_strerror(ret, errbuf, 1024);
            qWarning() << "error decoding" << errbuf;
            return;
        }
        qDebug() << "ret:" << ret << "pkt.size:" << pkt.size;

        if (got_frame){
            //saveFrameToImage(frame, m_outputPath, fmt_ctx->streams[video_stream_idx]->codec, skip_size*i);

            i++;
            qDebug() << skip_size*i << "/" << fmt_ctx->duration;
            /*if (av_seek_frame(fmt_ctx, -1, skip_size*i, 0) < 0) {
                qWarning() << "seek failed";
                break;
            }*/
            //avformat_seek_file(fmt_ctx, video_stream_idx, INT64_MIN, skip_size * i, INT64_MAX, 0);

            //avcodec_flush_buffers(fmt_ctx->streams[video_stream_idx]->codec);
        }
        av_frame_unref(frame);
        av_free_packet(&orig_packet);
    }
}
