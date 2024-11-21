import dbus
import dbus.mainloop.glib
from gi.repository import GLib

SERVER_BUS_NAME = "chung.simple"
SERVER_INTERFACE = "chung.simple.signal"
SIGNAL_NAME = "chung_signal"
CATCH_RULE = "type='signal',interface='chung.simple.signal'"
SERVER_OBJECT_PATH_NAME = "/chung/simple/signaling";

def signal_handler(message):
    print(f"Received signal with message: {message}")

def main():
    # Set up the main loop for D-Bus
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    # Connect to the session bus
    bus = dbus.SessionBus()

    # Define the interface and signal name to listen to
    bus.add_match_string_non_blocking(
        CATCH_RULE
    )

    # Add the signal receiver
    bus.add_signal_receiver(
        signal_handler,
        signal_name=SIGNAL_NAME,
        dbus_interface=SERVER_INTERFACE,
        path=SERVER_OBJECT_PATH_NAME
    )

    print("Listening for D-Bus signals...")

    # Run the main loop to process incoming D-Bus signals
    loop = GLib.MainLoop()
    loop.run()

if __name__ == "__main__":
    main()
