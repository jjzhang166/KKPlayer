#include "../Includeffmpeg.h"

#define RESYNC_BUFFER_SIZE (1<<20)
extern "C"{
typedef struct KKVContext {
	const AVClass *_class; ///< Class for private options.
	int trust_metadata;   ///< configure streams according onMetaData
	int wrong_dts;        ///< wrong dts due to negative cts

	int last_sample_rate;
	int last_channels;
	struct {
		int64_t dts;
		int64_t pos;
	} validate_index[2];
	int validate_next;
	int validate_count;
	int searched_for_end;

	uint8_t resync_buffer[2*RESYNC_BUFFER_SIZE];

	int broken_sizes;
	int sum_flv_tag_size;
} KKVContext;


#define OFFSET(x) offsetof(KKVContext, x)
#define VD AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_DECODING_PARAM
};
static const AVOption options[] = {
	{ "flv_metadata", "Allocate streams according to the onMetaData array", OFFSET(trust_metadata), AV_OPT_TYPE_INT, { (int64_t)0 }, 0, 1, VD },
	{ NULL }
};

static AVClass KKV_class;
void IniKKV_class()
{
	::memset(&KKV_class,0,sizeof(AVClass));
	KKV_class.class_name = "KKVdec";
	KKV_class.item_name  = av_default_item_name;
	KKV_class.option     = options;
	KKV_class.version    = LIBAVUTIL_VERSION_INT;

}


static int probe(AVProbeData *p)
{
	const uint8_t *d = p->buf;
	unsigned offset = AV_RB32(d + 5);

	if (d[0] == 'K' &&
		d[1] == 'K' &&
		d[2] == 'V')
	{				return AVPROBE_SCORE_MAX;
	}
	return AVPROBE_SCORE_MAX;
}

static int kkv_probe(AVProbeData *p)
{
	return probe(p);
}


static int kkv_read_header(AVFormatContext *s)
{
	KKVContext *flv =(KKVContext *) s->priv_data;
	int offset;

	avio_skip(s->pb, 4);
	avio_r8(s->pb); // flags

	s->ctx_flags |= AVFMTCTX_NOHEADER;

	offset = avio_rb32(s->pb);
	avio_seek(s->pb, offset, SEEK_SET);
	avio_skip(s->pb, 4);

	s->start_time = 0;
	flv->sum_flv_tag_size = 0;

	return 0;
}


static int kkv_read_packet(AVFormatContext *s, AVPacket *pkt)
{
	KKVContext *flv = (KKVContext *)s->priv_data;
	int i=0;
	i++;
	return 0;
}

static int kkv_read_seek(AVFormatContext *s, int stream_index,
						 int64_t ts, int flags)
{
	KKVContext *flv = (KKVContext *)s->priv_data;
	flv->validate_count = 0;
	int i=0;
	i++;
	return avio_seek_time(s->pb, stream_index, ts, flags);
}


static int kkv_read_close(AVFormatContext *s)
{
	
	KKVContext *flv = (KKVContext *)s->priv_data;
	int i=0;
	i++;
/*	for (i=0; i<FLV_STREAM_TYPE_NB; i++)
		av_freep(&flv->new_extradata[i]);*/
	return 0;
}

#if CONFIG_SMALL
#   define NULL_IF_CONFIG_SMALL(x) NULL
#else
#   define NULL_IF_CONFIG_SMALL(x) x
#endif

AVInputFormat ff_kk_demuxer;
void Inikk_demuxer()
{
	::memset(&ff_kk_demuxer,0,sizeof(ff_kk_demuxer));
	ff_kk_demuxer.name           = "kkv";
	ff_kk_demuxer.long_name      =  NULL_IF_CONFIG_SMALL("KK Video");
	ff_kk_demuxer.priv_data_size = sizeof(KKVContext);
	ff_kk_demuxer.read_probe     = kkv_probe;
	ff_kk_demuxer.read_header    = kkv_read_header;
	ff_kk_demuxer.read_packet    = kkv_read_packet;
	ff_kk_demuxer.read_seek      = kkv_read_seek;
	ff_kk_demuxer.read_close     = kkv_read_close;
	//ff_kk_demuxer.extensions     = "kkv";
	//ff_kk_demuxer.priv_class     = &KKV_class;
}

void register_Kkv()
{
	IniKKV_class();
	Inikk_demuxer();
	av_register_input_format(&ff_kk_demuxer);
}