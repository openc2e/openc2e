#ifndef SHARED_STR_H
#define SHARED_STR_H 1

class shared_str_b {
	FRIEND_SERIALIZE(shared_str_b)
	protected:
		std::string s;
	public:
		std::string &operator*() { return s; }
		const std::string &operator*() const { return s; }
		shared_str_b() { }
		shared_str_b(const std::string &p) : s(p) { }
};

class shared_str {
	FRIEND_SERIALIZE(shared_str)
	protected:
		std::shared_ptr<shared_str_b> p;
	public:
		shared_str() : p(new shared_str_b()) { }
		shared_str(const shared_str &v) : p(v.p) { }
		shared_str(const std::string &p) : p(new shared_str_b(p)) { }

		std::string &operator*() { return **p; }
		const std::string &operator*() const { return **p; }
		std::string *operator->() { return &(**this); }
		const std::string *operator->() const { return &(**this); }
};

#endif
