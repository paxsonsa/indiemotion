import threading
import indiemotion as idm

class Delegate:

    def on_server_start(self):
        pass

    def on_server_shutdown(self):
        pass

    def should_server_shutdown(self):
        return False

    def on_session_start(self, context: idm.context.Context):
        pass

    def on_session_close(self, context: idm.context.Context):
        pass

    def should_session_close(self):
        return False

def main():
    delegate = Delegate()
    server = idm.Server(
        server_name="my-server",
        port=5577,
        delegate=delegate
    )
    t = threading.Thread(server.start)
    t.start()
    t.join()
