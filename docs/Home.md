# Ermiry Test API

Playground to test CI/CD workflows and features with a test API written in C using [Cerver](https://github.com/ermiry/cerver)

### Development
```
sudo docker run \
  -it \
  --name test --rm \
  -p 5000:5000 \
  -v /home/ermiry/Documents/ermiry/Projects/ermiry-test-api:/home/test \
  -e RUNTIME=development \
  -e PORT=5000 \
  -e CERVER_RECEIVE_BUFFER_SIZE=4096 -e CERVER_TH_THREADS=4 \
  -e CERVER_CONNECTION_QUEUE=4 \
  -e MONGO_APP_NAME=api -e MONGO_DB=test \
  -e MONGO_URI=mongodb://api:password@192.168.100.39:27017/test \
  -e PRIV_KEY=/home/test/keys/key.key -e PUB_KEY=/home/test/keys/key.pub \
  ermiry/test-api:development /bin/bash
```

### Test
```
sudo docker run \
  -it \
  --name test --rm \
  -p 5000:5000 \
  -e RUNTIME=test \
  -e PORT=5000 \
  -e CERVER_RECEIVE_BUFFER_SIZE=4096 -e CERVER_TH_THREADS=4 \
  -e CERVER_CONNECTION_QUEUE=4 \
  -e MONGO_APP_NAME=api -e MONGO_DB=test \
  -e MONGO_URI=mongodb://api:password@192.168.100.39:27017/test \
  -e PRIV_KEY=/home/test/keys/key.key -e PUB_KEY=/home/test/keys/key.pub \
  ermiry/test-api:test /bin/bash
```