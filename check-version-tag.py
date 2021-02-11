import semantic_version as semver, sys
import re
import shutil, glob, os
from tempfile import mkstemp
from sh import sed

pre_release_release_list = ["test", "rc", "beta", "alpha"]

# vertical = None
# 1.0.0-test.3
# 1.0.0-alpha.1
# 1.0.0-beta.2
# 1.0.0-rc.1
# 1.0.0

def create_file(filename, content):
    file = open(filename, "w+")
    file.write(content)
    file.close()

def lsbfiles(dir):
   # note you have subdirs and files flipped in your code
   for root, subdirs, files in os.walk(dir):
      for file in files:
         if file.startswith('lsb'):
            yield(os.path.join(root,file))

if __name__ == '__main__':
    # get all arguments
    args = sys.argv
    # global vertical
    # first argument is the tag name, it represents the version number.
    tag = args[1]
    v = semver.Version(tag)

    # print("release is : "+str(v.major)+'.'+str(v.minor)+'.'+str(v.patch))

    try:
        if v.prerelease[0] in pre_release_release_list:
            # print("pre-release is : "+v.prerelease[0]+'.'+v.prerelease[1])
            # print("pre-release is : "+v.prerelease[0]+'.'+re.sub("-.*$", "",  v.prerelease[1]))
            pass
    except IndexError:
        pass

    try:
        print("export PRE_RELEASE='{}'".format(v.prerelease[0]))
    except IndexError:
        print("export PRE_RELEASE='{}'".format(""))

    exit(0)
