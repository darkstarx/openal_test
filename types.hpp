#ifndef TYPES_HPP
#define TYPES_HPP


struct position_t
{
	float x;
	float y;
	float z;
	position_t() : x(0.0f), y(0.0f), z(0.0f) {}
	explicit position_t(float x, float y) : x(x), y(y), z(0.0f) {}
	explicit position_t(float x, float y, float z) : x(x), y(y), z(z) {}
	~position_t() {}
	inline bool operator !() const { return x == 0.0f && y == 0.0f && z == 0.0f; }
	inline float* array() { return reinterpret_cast<float *>(this); }
};

inline bool operator ==(const position_t &lhs, const position_t &rhs)
{ return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z; }

inline bool operator !=(const position_t &lhs, const position_t &rhs)
{ return !(lhs == rhs); }

#endif // TYPES_HPP
