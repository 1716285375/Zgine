#pragma once

#include <chrono>

namespace Zgine {

/**
 * @class Timestep
 * @brief Represents a time step for frame timing and animation
 * @details Provides high-precision timing functionality for game loops,
 *          animation systems, and physics calculations
 */
class Timestep {
public:
    /**
     * @brief Constructor with time value in seconds
     * @param time Time value in seconds
     */
    Timestep(float time = 0.0f) : m_Time(time) {}

    /**
     * @brief Get time in seconds
     * @return Time value in seconds
     */
    float GetSeconds() const { return m_Time; }

    /**
     * @brief Get time in milliseconds
     * @return Time value in milliseconds
     */
    float GetMilliseconds() const { return m_Time * 1000.0f; }

    /**
     * @brief Get time in microseconds
     * @return Time value in microseconds
     */
    float GetMicroseconds() const { return m_Time * 1000000.0f; }

    /**
     * @brief Get time in nanoseconds
     * @return Time value in nanoseconds
     */
    float GetNanoseconds() const { return m_Time * 1000000000.0f; }

    /**
     * @brief Set time value in seconds
     * @param time Time value in seconds
     */
    void SetSeconds(float time) { m_Time = time; }

    /**
     * @brief Set time value in milliseconds
     * @param milliseconds Time value in milliseconds
     */
    void SetMilliseconds(float milliseconds) { m_Time = milliseconds / 1000.0f; }

    /**
     * @brief Set time value in microseconds
     * @param microseconds Time value in microseconds
     */
    void SetMicroseconds(float microseconds) { m_Time = microseconds / 1000000.0f; }

    /**
     * @brief Set time value in nanoseconds
     * @param nanoseconds Time value in nanoseconds
     */
    void SetNanoseconds(float nanoseconds) { m_Time = nanoseconds / 1000000000.0f; }

    /**
     * @brief Implicit conversion to float (seconds)
     * @return Time value in seconds
     */
    operator float() const { return m_Time; }

    /**
     * @brief Addition operator
     * @param other Another Timestep
     * @return Sum of the two timesteps
     */
    Timestep operator+(const Timestep& other) const {
        return Timestep(m_Time + other.m_Time);
    }

    /**
     * @brief Subtraction operator
     * @param other Another Timestep
     * @return Difference of the two timesteps
     */
    Timestep operator-(const Timestep& other) const {
        return Timestep(m_Time - other.m_Time);
    }

    /**
     * @brief Multiplication operator
     * @param scalar Scalar value
     * @return Timestep multiplied by scalar
     */
    Timestep operator*(float scalar) const {
        return Timestep(m_Time * scalar);
    }

    /**
     * @brief Division operator
     * @param scalar Scalar value
     * @return Timestep divided by scalar
     */
    Timestep operator/(float scalar) const {
        return Timestep(m_Time / scalar);
    }

    /**
     * @brief Addition assignment operator
     * @param other Another Timestep
     * @return Reference to this Timestep
     */
    Timestep& operator+=(const Timestep& other) {
        m_Time += other.m_Time;
        return *this;
    }

    /**
     * @brief Subtraction assignment operator
     * @param other Another Timestep
     * @return Reference to this Timestep
     */
    Timestep& operator-=(const Timestep& other) {
        m_Time -= other.m_Time;
        return *this;
    }

    /**
     * @brief Multiplication assignment operator
     * @param scalar Scalar value
     * @return Reference to this Timestep
     */
    Timestep& operator*=(float scalar) {
        m_Time *= scalar;
        return *this;
    }

    /**
     * @brief Division assignment operator
     * @param scalar Scalar value
     * @return Reference to this Timestep
     */
    Timestep& operator/=(float scalar) {
        m_Time /= scalar;
        return *this;
    }

    /**
     * @brief Equality operator
     * @param other Another Timestep
     * @return True if timesteps are equal
     */
    bool operator==(const Timestep& other) const {
        return m_Time == other.m_Time;
    }

    /**
     * @brief Inequality operator
     * @param other Another Timestep
     * @return True if timesteps are not equal
     */
    bool operator!=(const Timestep& other) const {
        return m_Time != other.m_Time;
    }

    /**
     * @brief Less than operator
     * @param other Another Timestep
     * @return True if this timestep is less than other
     */
    bool operator<(const Timestep& other) const {
        return m_Time < other.m_Time;
    }

    /**
     * @brief Greater than operator
     * @param other Another Timestep
     * @return True if this timestep is greater than other
     */
    bool operator>(const Timestep& other) const {
        return m_Time > other.m_Time;
    }

    /**
     * @brief Less than or equal operator
     * @param other Another Timestep
     * @return True if this timestep is less than or equal to other
     */
    bool operator<=(const Timestep& other) const {
        return m_Time <= other.m_Time;
    }

    /**
     * @brief Greater than or equal operator
     * @param other Another Timestep
     * @return True if this timestep is greater than or equal to other
     */
    bool operator>=(const Timestep& other) const {
        return m_Time >= other.m_Time;
    }

    /**
     * @brief Create a Timestep from seconds
     * @param seconds Time in seconds
     * @return Timestep object
     */
    static Timestep FromSeconds(float seconds) {
        return Timestep(seconds);
    }

    /**
     * @brief Create a Timestep from milliseconds
     * @param milliseconds Time in milliseconds
     * @return Timestep object
     */
    static Timestep FromMilliseconds(float milliseconds) {
        return Timestep(milliseconds / 1000.0f);
    }

    /**
     * @brief Create a Timestep from microseconds
     * @param microseconds Time in microseconds
     * @return Timestep object
     */
    static Timestep FromMicroseconds(float microseconds) {
        return Timestep(microseconds / 1000000.0f);
    }

    /**
     * @brief Create a Timestep from nanoseconds
     * @param nanoseconds Time in nanoseconds
     * @return Timestep object
     */
    static Timestep FromNanoseconds(float nanoseconds) {
        return Timestep(nanoseconds / 1000000000.0f);
    }

    /**
     * @brief Get current time as Timestep
     * @return Current time as Timestep
     */
    static Timestep Now() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        return FromNanoseconds(static_cast<float>(nanoseconds));
    }

    /**
     * @brief Get zero timestep
     * @return Zero timestep
     */
    static Timestep Zero() {
        return Timestep(0.0f);
    }

private:
    float m_Time; ///< Time value in seconds
};

} // namespace Zgine
