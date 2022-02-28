import sys
import os
import threading

import indiemotion


class SimpleDelegate:
    """ Delegate for IndieMotion

    Much like the C++ API, we interact with the indiemotion server runtime
    using a delegate pattern. Unlike the C++ API, this is done through a
    single instance of a python module.

    There are four categories of delegate methods you can implement.

        Server - These are methods that relate directly to the network
            server and its connections
        Session - These are that relate to the context of a given motion
            capture session.
        Scene - Within each session there is a scene context which keeps tracks
            of objects in the scene (e.g. cameras)
        Motion - Relates to the motion context of a session (e.g. motion data).
    """
    should_server_shutdown_called = False
    def should_server_shutdown(self):
        self.should_server_shutdown_called = True
        return False

    on_server_start_called = False
    def on_server_start(self):
        print("on_server_start")
        self.on_server_start_called = True
        pass

    on_server_shutdown_called = False
    def on_server_shutdown(self):
        print("on_server_shutdown")
        self.on_server_shutdown_called = True
        pass

    on_connection_start_called = False
    def on_connection_start(self):
        print("on_connection_start")
        self.on_connection_start_called = True
        pass

    should_connection_close_called = False
    def should_connection_close(self):
        self.should_connection_close_called = True
        return False

    on_connection_close_called = False
    def on_connection_close(self):
        print("on_connection_close")
        self.on_connection_close_called = True
        pass

    on_motion_updated_called = False
    def on_motion_updated(self, ctx):
        self.on_motion_updated_called = True
        print("Motion Updated: ", ctx)

    on_scene_updated_called = False
    def on_scene_updated(self, ctx):
        self.on_scene_updated_called = True
        print("Scene Updated: ", ctx)

    get_scene_cameras_called = False
    def get_scene_cameras(self, ctx):
        self.get_scene_cameras_called = True
        return [
            indiemotion.SceneCamera("cam1")
        ]

    on_session_startup_called = False
    def on_session_startup(self, ctx):
        self.on_session_startup_called = True
        print("Session Started: ", ctx)

    on_session_updated_called = False
    def on_session_updated(self, ctx):
        self.on_session_updated_called = True
        print("Session Updated: ", ctx)

    on_session_shutdown_called = False
    def on_session_shutdown(self, ctx):
        self.on_session_shutdown_called = True
        print("Session Shutdown: ", ctx)

    should_session_shutdown_called = False
    def should_session_shutdown(self, ctx):
        print("session shutdown: ", ctx)
        self.should_session_shutdown_called = True
        return False


class ServerThread(threading.Thread):

    def run(self):
        delegate = SimpleDelegate()
        server = indiemotion.Server(delegate)
        server.start_blocking()


def main():
    thread = ServerThread()
    thread.start()

    print("Server Started.")
    thread.join()


if __name__ == "__main__":
    print(sys.executable)
    main()
