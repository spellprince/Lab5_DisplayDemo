#include "GraphicsLibUDP.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <atomic>

int main()
{
    GraphicsLibUDP display(320, 240);

    if (!display.connectTo("127.0.0.1", 12345))
    {
        std::cout << "Не удалось подключиться к эмулятору!"
            << std::endl;
        return 1;
    }

    const uint16_t BACKGROUND = RGB(10, 18, 32);
    const uint16_t PANEL = RGB(18, 32, 50);
    const uint16_t CYAN = RGB(50, 220, 220);
    const uint16_t WHITE = RGB(255, 255, 255);
    const uint16_t GREEN = RGB(70, 230, 150);
    const uint16_t YELLOW = RGB(255, 210, 70);

    display.fillScreen(BACKGROUND);

    display.fillRect(12, 12, 296, 216, PANEL);
    display.drawRect(12, 12, 296, 216, CYAN);

    display.drawText(
        75, 30,
        "ELECTRONIC CLOCK",
        CYAN, PANEL, 2
    );

    display.drawLine(30, 65, 290, 65, CYAN);

    display.drawText(
        120, 82,
        "CURRENT TIME",
        WHITE, PANEL, 1
    );

    display.drawText(
        120, 155,
        "DATE",
        WHITE, PANEL, 1
    );

    std::atomic<bool> running(true);

    std::thread stopThread([&running]()
        {
            std::cout << "Нажмите Enter для завершения программы."
                << std::endl;

            std::cin.get();
            running = false;
        });

    auto nextUpdate = std::chrono::steady_clock::now();

    while (running)
    {
        auto now = std::chrono::system_clock::now();

        std::time_t currentTime =
            std::chrono::system_clock::to_time_t(now);

        std::tm localTime = {};
        localtime_s(&localTime, &currentTime);

        char timeBuffer[16];
        char dateBuffer[32];

        std::strftime(
            timeBuffer,
            sizeof(timeBuffer),
            "%H:%M:%S",
            &localTime
        );

        std::strftime(
            dateBuffer,
            sizeof(dateBuffer),
            "%d.%m.%Y",
            &localTime
        );

        display.fillRect(35, 105, 250, 38, PANEL);

        display.drawText(
            82, 108,
            timeBuffer,
            GREEN, PANEL, 5
        );

        display.fillRect(65, 177, 190, 30, PANEL);

        display.drawText(
            105, 180,
            dateBuffer,
            YELLOW, PANEL, 2
        );

        nextUpdate += std::chrono::seconds(1);

        std::this_thread::sleep_until(nextUpdate);
    }

    if (stopThread.joinable())
        stopThread.join();

    display.fillScreen(BACKGROUND);
    display.disconnect();

    return 0;
}