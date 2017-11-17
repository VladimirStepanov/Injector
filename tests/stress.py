import os
import sys
import urllib
import hashlib
import threading
import subprocess



def md5(fname):
    hash_md5 = hashlib.md5()
    with open(fname, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return str(hash_md5.hexdigest())


def test(url, real_md5, count):

    for i in range(count):
        fname = "test{0}".format(i)
        if not os.fork():
            testfile = urllib.URLopener()
            testfile.retrieve(url, fname)
            if md5(fname) != real_md5:
                print( "FAIL{0} {1} != {2}".format(i, real_md5, md5(fname)))
            else:
                print( "SUCCESS{0}".format(i))

            os.remove(fname)
            exit(0)



if __name__ == '__main__':

    if len(sys.argv) != 4:
        print ("Usage: ./{0} url original_file_md5sum download_count".format(sys.argv[0]))
        exit(0)
    try:
        test(sys.argv[1], sys.argv[2], int(sys.argv[3]))
    except ValueError:
        print ("count must be integer")
