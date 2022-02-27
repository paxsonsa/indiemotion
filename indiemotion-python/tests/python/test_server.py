import os
import unittest

import indiemotion

class SimpleDelegate:

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


class ServerTesting(unittest.TestCase):

    # @unittest.skipUnless(os.environ.get("INDIEMOTION_TEST_SERVER"), "requires $INDIEMOTION_TEST_SERVER to be set")
    def test_server_integration(self):
        print(os.environ)
        delegate = SimpleDelegate()
        server = indiemotion.Server(delegate)
        server.start_blocking()

if __name__ == '__main__':
    unittest.main()
