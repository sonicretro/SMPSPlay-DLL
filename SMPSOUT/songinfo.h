enum TrackMode {
	TrackMode_SK,
	TrackMode_S3,
	TrackMode_S3D,
	TrackMode_S1,
	TrackMode_S2B,
	TrackMode_S2
};

struct musicentry { unsigned short base; unsigned char mode; };

extern const musicentry MusicFiles[];

#if ! defined(_MSC_VER) || _MSC_VER >= 1600
template <typename T, size_t N>
inline size_t LengthOfArray(const T(&)[N])
{
	return N;
}
#else
#define LengthOfArray(x)	(sizeof(x) / sizeof((x)[0]))
#endif

struct trackoption { const char *text; char id; };

struct tracknameoptions { const char *name; const trackoption *options; int optioncount; };

#define arrayptrandlength(x) x, LengthOfArray(x)

#define trackoptdef(x) { #x, arrayptrandlength(x##Options) }

extern const tracknameoptions TrackOptions[];