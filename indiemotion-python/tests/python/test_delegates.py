import unittest
import indiemotion


class DelegateSpy:

    on_motion_updated_called = False
    def on_motion_updated(self, ctx):
        self.on_motion_updated_called = True

    on_scene_updated_called = False
    def on_scene_updated(self, ctx):
        self.on_scene_updated_called = True

    get_scene_cameras_called = False
    def get_scene_cameras(self, ctx):
        self.get_scene_cameras_called = True
        return [
            indiemotion.SceneCamera("cam1")
        ]

    on_session_startup_called = False
    def on_session_startup(self, ctx):
        self.on_session_startup_called = True

    on_session_updated_called = False
    def on_session_updated(self, ctx):
        self.on_session_updated_called = True

    on_session_shutdown_called = False
    def on_session_shutdown(self, ctx):
        self.on_session_shutdown_called = True

    should_session_shutdown_called = False
    def should_session_shutdown(self, ctx):
        self.should_session_shutdown_called = True
        return False


class TestDelegateClasses(unittest.TestCase):

    def setUp(self) -> None:
        self.delegate = DelegateSpy()

    def test_default_motion_context_callers(self):
        ctx = indiemotion.Context()
        self.delegate.on_motion_updated(ctx)
        self.assertTrue(self.delegate.on_motion_updated_called)

    def test_default_scene_context_callers(self):
        ctx = indiemotion.Context()

        self.delegate.on_scene_updated(ctx)
        self.assertTrue(self.delegate.on_scene_updated_called)

        self.delegate.get_scene_cameras(ctx)
        self.assertTrue(self.delegate.get_scene_cameras_called)


class TestDelegateValidator(unittest.TestCase):

    def setUp(self) -> None:
        self.delegate = DelegateSpy()

    def test_validate_scene_delegate(self):
        ctx = indiemotion.Context()
        indiemotion.test.validate_scene_delegate(ctx, self.delegate)
        self.assertTrue(self.delegate.get_scene_cameras_called)
        self.assertTrue(self.delegate.on_scene_updated_called)

    def test_validate_motion_delegate(self):
        ctx = indiemotion.Context()
        indiemotion.test.validate_motion_delegate(ctx, self.delegate)
        self.assertTrue(self.delegate.on_motion_updated_called)

    def test_validate_session_delegate(self):
        ctx = indiemotion.Context()
        indiemotion.test.validate_session_delegate(ctx, self.delegate)
        self.assertTrue(self.delegate.on_session_startup_called)
        self.assertTrue(self.delegate.on_session_updated_called)
        self.assertTrue(self.delegate.on_session_shutdown_called)
        self.assertTrue(self.delegate.should_session_shutdown_called)
