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
