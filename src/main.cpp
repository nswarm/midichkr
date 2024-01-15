#include <RtMidi.h>
#include <fmt/format.h>
#include <string_view>
#include <csignal>
#include <thread>

using namespace std::chrono_literals;

template <typename... T>
void log(std::string format, T&&... args) {
    format = format.append("\n");
    fmt::print(format, args...);
}
void log() {
    log("");
}

bool done;
static void exit(){ done = true; }
void create_sigint() {
    done = false;
    (void) signal(SIGINT, exit);
}

void run() {
    RtMidiIn in;

    log("discovered devices:");
    auto ports = in.getPortCount();
    for (int i = 0; i < ports; ++i) {
        log("  {}: {}", i, in.getPortName(i));
    }

    if (ports == 0) {
        return;
    }
    if (ports > 1) {
        log("multiple devices available, only the 0th device will be used...");
    }

    log();
    log("reading from device: {}...", in.getPortName(0));
    in.openPort(0);
    create_sigint();

    in.ignoreTypes(false, true, true);

    std::vector<unsigned char> message;
    while (!done) {
        in.getMessage(&message);
        if (!message.empty()) {
            for (unsigned char i: message) {
                fmt::print("{0:#x} ", i);
            }
            std::cout << std::endl;
        }
        std::this_thread::sleep_for(10ms);
    }
}

int main() {
    try {
        run();
    } catch (RtMidiError& error) {
        error.printMessage();
    }
    log("press enter to close...");
    std::cin.get();
    return 0;
}
