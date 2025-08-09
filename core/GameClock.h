#pragma once
#include <cstdint>

class GameClock {
public:
    static constexpr int SHICHEN_MS = 150000; // 2.5 minutes
    static constexpr int DAY_MS = SHICHEN_MS * 12;

    void advance(int ms) {
        realMs_ += ms;
        dayMs_ += ms;
        while (dayMs_ >= DAY_MS) {
            dayMs_ -= DAY_MS;
            dayCount_++;
        }
    }

    int shichen() const { return dayMs_ / SHICHEN_MS; }
    int dayCount() const { return dayCount_; }
    int dayMs() const { return dayMs_; }
    void set(int day, int ms) {
        dayCount_ = day;
        dayMs_ = ms % DAY_MS;
    }
    int64_t realMs() const { return realMs_; }
private:
    int64_t realMs_ = 0;
    int dayMs_ = 0;
    int dayCount_ = 1;
};
