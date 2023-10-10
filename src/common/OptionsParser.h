#pragma once

#include "StringView.h"

template <typename T>
struct OptImpl;

template <>
struct OptImpl<bool> {
	StringView name;
	bool* out;

	void print_usage(FILE* f) const {
		fmt::print(f, " [{}]", name);
	}

	bool try_parse(int, char** argv, int argidx) const {
		if (StringView(argv[argidx]) == name) {
			*out = true;
			return true;
		}
		return false;
	}
};

template <>
struct OptImpl<std::string> {
	StringView name;
	std::string* out;

	void print_usage(FILE* f) const {
		fmt::print(f, " {}", name);
	}

	bool try_parse(int argc, char** argv, int argidx) const {
		// a single final positional argument
		if (argc - argidx == 1) {
			*out = argv[argidx];
			return true;
		}
		return false;
	}
};

struct OptHolder {
	virtual ~OptHolder() = default;
	virtual void print_usage(FILE* f) = 0;
	virtual bool try_parse(int argc, char** argv, int argidx) = 0;
};

template <typename T>
struct OptHolderImpl : OptHolder {
	OptHolderImpl(OptImpl<T> t_)
		: t(t_) {}
	OptImpl<T> t;
	void print_usage(FILE* f) override {
		return t.print_usage(f);
	}
	bool try_parse(int argc, char** argv, int argidx) override {
		return t.try_parse(argc, argv, argidx);
	}
};

struct OptionsParser {
	const char* argv0 = nullptr;
	std::vector<std::shared_ptr<OptHolder>> opts;

	template <typename T>
	OptionsParser opt(StringView name, T* out) {
		// break up the lines for better error messages
		auto o = new OptHolderImpl<T>(OptImpl<T>{name, out});
		opts.emplace_back(std::shared_ptr<OptHolder>(o));
		return *this;
	}

	OptionsParser parse(int argc, char** argv) {
		argv0 = argv[0];

		bool error = false;
		bool help = false;
		int argidx = 1;
		while (argidx < argc) {
			if (std::string(argv[argidx]) == "--help" || std::string(argv[argidx]) == "-h") {
				help = true;
				argidx++;
				continue;
			}
			bool parsed = false;
			for (const auto& o : opts) {
				if (o->try_parse(argc, argv, argidx)) {
					argidx++;
					parsed = true;
					break;
				}
			}
			if (parsed) {
				continue;
			}
			fmt::print(stderr, "Unknown argument: {}\n", argv[argidx]);
			error = true;
			argidx++;
		}

		if (error || help) {
			print_usage(help ? stdout : stderr);
			exit(help ? 0 : 1);
		}

		return *this;
	}

	void print_usage(FILE* f = stdout) {
		fmt::print(f, "Usage: {}", argv0 ? argv0 : "(null-argv0)");
		for (const auto& o : opts) {
			o->print_usage(f);
		}
		fmt::print("\n");
		fflush(f);
	}

	void print_usage_and_fail() {
		print_usage(stderr);
		exit(1);
	}
};