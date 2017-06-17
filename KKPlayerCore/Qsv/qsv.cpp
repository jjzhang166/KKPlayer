
extern "C"{
#include <mfx/mfxvideo.h>
#include <mfx/mfxplugin.h>

#include <stdio.h>
#include <string.h>

#include "libavutil/avstring.h"
#include "libavutil/error.h"
}
#include "qsv.h"
#include "qsv_internal.h"

int kk_qsv_map_pixfmt(enum AVPixelFormat format, uint32_t *fourcc)
{
    switch (format) {
    case AV_PIX_FMT_YUV420P:
    case AV_PIX_FMT_YUVJ420P:
    case AV_PIX_FMT_NV12:
        *fourcc = MFX_FOURCC_NV12;
        return AV_PIX_FMT_NV12;
    case AV_PIX_FMT_YUV420P10:
    case AV_PIX_FMT_P010:
        *fourcc = MFX_FOURCC_P010;
        return AV_PIX_FMT_P010;
    default:
        return AVERROR(ENOSYS);
    }
}
int ff_qsv_codec_id_to_mfx(enum AVCodecID codec_id)
{
    switch (codec_id) {
    case AV_CODEC_ID_H264:
        return MFX_CODEC_AVC;

    case AV_CODEC_ID_HEVC:
        return MFX_CODEC_HEVC;

    case AV_CODEC_ID_MPEG1VIDEO:
    case AV_CODEC_ID_MPEG2VIDEO:
        return MFX_CODEC_MPEG2;
    case AV_CODEC_ID_VC1:
        return MFX_CODEC_VC1;
    default:
        break;
    }

    return AVERROR(ENOSYS);
}
int kk_qsv_profile_to_mfx(enum AVCodecID codec_id, int profile)
{
    if (profile == FF_PROFILE_UNKNOWN)
        return MFX_PROFILE_UNKNOWN;
    switch (codec_id) {
    case AV_CODEC_ID_H264:
    case AV_CODEC_ID_HEVC:
        return profile;
    case AV_CODEC_ID_VC1:
        return 4 * profile + 1;
    case AV_CODEC_ID_MPEG2VIDEO:
        return 0x10 * profile;
    }
    return MFX_PROFILE_UNKNOWN;
}
int ff_qsv_error(int mfx_err)
{
    switch (mfx_err) {
    case MFX_ERR_NONE:
        return 0;
    case MFX_ERR_MEMORY_ALLOC:
    case MFX_ERR_NOT_ENOUGH_BUFFER:
        return AVERROR(ENOMEM);
    case MFX_ERR_INVALID_HANDLE:
        return AVERROR(EINVAL);
    case MFX_ERR_DEVICE_FAILED:
    case MFX_ERR_DEVICE_LOST:
    case MFX_ERR_LOCK_MEMORY:
        return AVERROR(EIO);
    case MFX_ERR_NULL_PTR:
    case MFX_ERR_UNDEFINED_BEHAVIOR:
    case MFX_ERR_NOT_INITIALIZED:
        return AVERROR_BUG;
    case MFX_ERR_UNSUPPORTED:
    case MFX_ERR_NOT_FOUND:
        return AVERROR(ENOSYS);
    case MFX_ERR_MORE_DATA:
    case MFX_ERR_MORE_SURFACE:
    case MFX_ERR_MORE_BITSTREAM:
        return AVERROR(EAGAIN);
    case MFX_ERR_INCOMPATIBLE_VIDEO_PARAM:
    case MFX_ERR_INVALID_VIDEO_PARAM:
        return AVERROR(EINVAL);
    case MFX_ERR_ABORTED:
    case MFX_ERR_UNKNOWN:
    default:
        return AVERROR_UNKNOWN;
    }
}

/**
 * @brief Initialize a MSDK session
 *
 * Media SDK is based on sessions, so this is the prerequisite
 * initialization for HW acceleration.  For Windows the session is
 * complete and ready to use, for Linux a display handle is
 * required.  For releases of Media Server Studio >= 2015 R4 the
 * render nodes interface is preferred (/dev/dri/renderD).
 * Using Media Server Studio 2015 R4 or newer is recommended
 * but the older /dev/dri/card interface is also searched
 * for broader compatibility.
 *
 * @param avctx    ffmpeg metadata for this codec context
 * @param session  the MSDK session used
 */
int ff_qsv_init_internal_session(AVCodecContext *avctx,KKQSVSession *qs,
                                 const char *load_plugins)
{
    mfxIMPL impl   = MFX_IMPL_AUTO_ANY;
    mfxVersion ver = { { 1, 0 } };

    const char *desc;
    int ret;

    ret = MFXInit(impl, &ver, &qs->session);
    if (ret < 0) {
        av_log(avctx, AV_LOG_ERROR, "Error initializing an internal MFX session\n");
        return ff_qsv_error(ret);
    }

    ret = 0;
    if (ret < 0)
        return ret;

    if (load_plugins && *load_plugins) {
        while (*load_plugins) {
            mfxPluginUID uid;
            int i, err = 0;

            char *plugin = av_get_token(&load_plugins, ":");
            if (!plugin)
                return AVERROR(ENOMEM);
            if (strlen(plugin) != 2 * sizeof(uid.Data)) {
                av_log(avctx, AV_LOG_ERROR, "Invalid plugin UID length\n");
                err = AVERROR(EINVAL);
                goto load_plugin_fail;
            }

            for (i = 0; i < sizeof(uid.Data); i++) {
                err = sscanf(plugin + 2 * i, "%2hhx", uid.Data + i);
                if (err != 1) {
                    av_log(avctx, AV_LOG_ERROR, "Invalid plugin UID\n");
                    err = AVERROR(EINVAL);
                    goto load_plugin_fail;
                }

            }

            ret = MFXVideoUSER_Load(qs->session, &uid, 1);
            if (ret < 0) {
                av_log(avctx, AV_LOG_ERROR, "Could not load the requested plugin: %s\n",
                       plugin);
                err = ff_qsv_error(ret);
                goto load_plugin_fail;
            }

            if (*load_plugins)
                load_plugins++;
load_plugin_fail:
            av_freep(&plugin);
            if (err < 0)
                return err;
        }
    }

    MFXQueryIMPL(qs->session, &impl);

    switch (MFX_IMPL_BASETYPE(impl)) {
    case MFX_IMPL_SOFTWARE:
        desc = "software";
        break;
    case MFX_IMPL_HARDWARE:
    case MFX_IMPL_HARDWARE2:
    case MFX_IMPL_HARDWARE3:
    case MFX_IMPL_HARDWARE4:
        desc = "hardware accelerated";
        break;
    default:
        desc = "unknown";
    }

    av_log(avctx, AV_LOG_VERBOSE,
           "Initialized an internal MFX session using %s implementation\n",
           desc);

    return 0;
}

int ff_qsv_close_internal_session(KKQSVSession *qs)
{
    if (qs->session) {
        MFXClose(qs->session);
        qs->session = NULL;
    }
    return 0;
}
