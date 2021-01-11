# Mongo

Authenticate as the db admin

```
use admin

db.auth("admin", passwordPrompt())
```

Select the db

```
use test
```

Create Mongo DB user

```
db.createUser(
    {
      user: "api",
      pwd:  "password",
      roles: [ { role: "readWrite", db: "test" }]
    }
)
```