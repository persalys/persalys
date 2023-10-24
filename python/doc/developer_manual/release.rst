.. _release:

Release process
===============

1. Update translations

2. Increment version number X.Y (utils/setVersionNumber.sh)

3. Update Changelog

4. Update distro/windows/persalys-doc.zip

5. Upload tagged docker images (docker push)

6. Create X.Y branch and protect it

7. In X.Y branch, modify .gitlab-ci.yml to used tagged docker images

8. In X.Y branch, create vX.Y tag

9. Push X.Y branch, vX.Y tag

10. Sync github mirror::

      git remote add gh git@github.com:persalys/persalys.git
      git push gh master
      git push gh --tags
