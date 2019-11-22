enum TrackMode {
	TrackMode_SK,
	TrackMode_S3,
	TrackMode_S3D,
	TrackMode_S1,
	TrackMode_S2B,
	TrackMode_S2,
	TrackMode_S3P,
};

struct musicentry { unsigned short base; unsigned char mode; const char *name; };

extern const musicentry MusicFiles[];
