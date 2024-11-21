#include <stdexcept>
#include <exception>
#include <functional>

struct SpeedUpdate
{
    double velocity_mps;
};

struct CarDetected
{
    double distance_m;
    double velocity_mps;
};

struct BrakeCommand
{
    double time_to_collision_s;
};

struct ServiceBus
{
    void publish(const BrakeCommand&);
};

using SpeedUpdateCallback = std::function<void(const SpeedUpdate&)>;
using CarDetectedCallback = std::function<void(const CarDetected&)>;

struct IServiceBus
{
    virtual ~IServiceBus() = default;
    virtual void publish(const BrakeCommand&) = 0;
    virtual void subscribe(SpeedUpdateCallback) = 0;
    virtual void subscribe(CarDetectedCallback) = 0;
};

struct MockServiceBus : IServiceBus
{
    void publish(const BrakeCommand& cmd) override
    {
        m_command_published++;
        m_last_cmd = cmd;
    }

    void subscribe(SpeedUpdateCallback callback) override
    {
        m_speed_update_callback = callback;
    }

    void subscribe(CarDetectedCallback callback)  override
    {
        m_car_detected_callback = callback;
    }

    int m_command_published{};
    BrakeCommand m_last_cmd{};
    SpeedUpdateCallback m_speed_update_callback{};
    CarDetectedCallback m_car_detected_callback{};
};

// template<typename T>
struct AutoBreak
{
    AutoBreak(IServiceBus& bus)
        : _collision_threshold_s{ 5 }
        , _speed_mps{}
        // , _publish{publish}
    {
        bus.subscribe([this](const SpeedUpdate& update)
            {
                _speed_mps = update.velocity_mps;
            });

        bus.subscribe([this, &bus](const CarDetected& cd) {
            const auto relative_vel_mps = _speed_mps - cd.velocity_mps;
            const auto time_to_collision_s = cd.distance_m / relative_vel_mps;
            if (time_to_collision_s > 0 && time_to_collision_s <= _collision_threshold_s)
            {
                bus.publish(BrakeCommand{ time_to_collision_s });   
            }
            });
    }
    void set_collision_threshold_s(double x)
    {
        if (x < 1) throw std::logic_error{ "Collision threshold less than 1." };
        _collision_threshold_s = x;
    }
    double get_collision_threshold_s() const
    {
        return _collision_threshold_s;
    }
    double get_speed_mps() const
    {
        return _speed_mps;
    }
private:
    double _collision_threshold_s;
    double _speed_mps;
    // const T& _publish;
};

