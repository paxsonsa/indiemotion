import unittest
import indiemotion


class TestSceneContext(unittest.TestCase):
    def test_active_camera(self):

        ctx = indiemotion.SceneContext()
        self.assertEqual(ctx.active_camera, None)

        ctx.active_camera = "cam1"
        self.assertEqual(ctx.active_camera, "cam1")

    def test_cameras(self):
        ctx = indiemotion.SceneContext()
        self.assertEqual(ctx.cameras, [])
        ctx.cameras.append(
            indiemotion.Camera("cam1")
        )

        self.assertEqual(len(ctx.cameras), 1)
        self.assertEqual(ctx.cameras[0], indiemotion.Camera("cam1"))

        ctx.cameras = []
        self.assertEqual(ctx.cameras, [])


class TestMotionContext(unittest.TestCase):

    def test_motion_status(self):
        ctx = indiemotion.MotionContext()
        self.assertEqual(ctx.status, indiemotion.motion_status.idle)

        ctx.status = indiemotion.motion_status.live
        self.assertEqual(ctx.status, indiemotion.motion_status.live)

    def test_motion_xform(self):
        ctx = indiemotion.MotionContext()
        self.assertEqual(ctx.current_xform.translation, indiemotion.Vector3())
        self.assertEqual(ctx.current_xform.orientation, indiemotion.Vector3())

        self.assertEqual(ctx.current_xform.translation.x, 0.0)
        self.assertEqual(ctx.current_xform.translation.y, 0.0)
        self.assertEqual(ctx.current_xform.translation.z, 0.0)
        self.assertEqual(ctx.current_xform.orientation.x, 0.0)
        self.assertEqual(ctx.current_xform.orientation.y, 0.0)
        self.assertEqual(ctx.current_xform.orientation.z, 0.0)

        t = indiemotion.Vector3()
        t.x = 1.0
        t.y = 2.0
        t.z = 3.0
        o = indiemotion.Vector3()
        o.x = 4.0
        o.y = 5.0
        o.z = 6.0
        ctx.current_xform.translation = t
        ctx.current_xform.orientation = o

        self.assertEqual(ctx.current_xform.translation.x, 1.0)
        self.assertEqual(ctx.current_xform.translation.y, 2.0)
        self.assertEqual(ctx.current_xform.translation.z, 3.0)
        self.assertEqual(ctx.current_xform.orientation.x, 4.0)
        self.assertEqual(ctx.current_xform.orientation.y, 5.0)
        self.assertEqual(ctx.current_xform.orientation.z, 6.0)


if __name__ == '__main__':
    unittest.main()
