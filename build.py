from halo import Halo
import time

spinner = Halo(text='Building kernel', spinner='dots')
spinner.start()

time_start = time.time()
time.sleep(1)
time_end = time.time()
spinner.succeed("Done in %ds" % (time_end - time_start))
time.sleep(2)

spinner.stop()
