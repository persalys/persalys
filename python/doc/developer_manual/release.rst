.. _release:

Release process
===============

1. Update translations (make sure everything is green)

2. Set version number X.Y (utils/setVersionNumber.sh X.Y)

3. Update Changelog

4. Update distro/windows/persalys-doc.zip

5. Upload tagged docker images (utils/docker_push.sh X.Y)

6. Create X.Y branch and protect it

7. In X.Y branch, modify .gitlab-ci.yml to add X.Y version argument to build_locally.sh to use tagged images

8. In X.Y branch, create vX.Y tag

9. Push X.Y branch, vX.Y tag

10. Sync github code mirror::

      git remote add gh git@github.com:persalys/persalys.git
      git push gh master
      git push gh --tags

11. Sync documentation repo::

    git clone git@github.com:persalys/persalys.github.io.git
    cd persalys.github.io
    git rm persalys/latest -r
    mv ../persalys/html persalys/latest # copy from local html files
    cp -r persalys/latest persalys/X.Y
    git add persalys/latest persalys/X.Y -r
    git commit -am "latest -> X.Y"

12. In master, set version X.(Y+1)dev
