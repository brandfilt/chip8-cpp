#ifndef __TIMER_H_
#define __TIMER_H_


class Timer {
public:
  Timer(int frequency) : m_frequency(frequency), m_counter(0), m_value(0) {}
  Timer &operator=(uint8_t value) {
    m_value = value;
    return *this;
  }

  uint8_t value() const { return m_value; }

  void setValue(int value) { m_value = value; }

  void update(uint16_t clock_speed) {
    m_counter++;

    if (m_counter >= m_frequency / clock_speed) {
      m_value = m_value > 0 ? --m_value : 0;
      m_counter = 0;
    }
  }

private:
  int m_frequency;
  int m_counter;
  uint8_t m_value;
};

#endif // __TIMER_H_
