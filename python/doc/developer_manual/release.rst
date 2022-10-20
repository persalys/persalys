.. _release:

Release process
===============

1. Increment version number X.Y (utils/setVersionNumber.sh)

2. Update Changelog

3. Update distro/windows/persalys-doc.zip

4. Upload tagged docker images (docker push)

5. Create X.Y branch

6. In X.Y branch, modify .gitlab-ci.yml to used tagged docker images

7. In X.Y branch, create vX.Y tag

8. Push X.Y branch, vX.Y tag
