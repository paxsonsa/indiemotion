import unittest
import indiemotion
import indiemotion_test

class Delegate:

    def should_close_connection(self):
        return False

    def on_init(self, ctx):
        print(ctx)

    def on_shutdown(self, ctx):
        print(ctx)

    def on_scene_update(self, ctx):
        print(ctx)

    def get_scene_cameras(self):
        return []

    def on_session_update(self, ctx):
        print(ctx)

    def on_motion_update(self, ctx):
        print(ctx)

class TestContextDelegateCaller(unittest.TestCase):
    def test_delegate_function_is_invoke(self):
        delegate = Delegate()

        indiemotion.test.delegate_integration()

