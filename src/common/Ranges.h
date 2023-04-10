#pragma once

template <typename R, typename Pred>
void erase_if(R&& r, Pred pred) {
	for (auto it = r.begin(); it != r.end();) {
		if (pred(*it)) {
			it = r.erase(it);
		} else {
			++it;
		}
	}
}

template <typename R, typename Pred>
int32_t index_if(const R& r, Pred pred) {
	int32_t i = 0;
	auto it = r.begin();
	const auto end = r.end();
	for (; it != end; ++it, ++i) {
		if (pred(*it)) {
			return i;
		}
	}
	return -1;
}