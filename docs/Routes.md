# Routes

### GET /api/test
**Access:** Public \
**Description:** Test API top level route \
**Returns:**
  - 200 on success

### GET api/test/version
**Access:** Public \
**Description:** Returns test service current version \
**Returns:**
  - 200 and version's json on success

### GET api/test/auth
**Access:** Private \
**Description:** Used to test if jwt keys work correctly \
**Returns:**
  - 200 on success
  - 401 on failed auth

### GET /api/users
**Access:** Public \
**Description:** Users top level route \
**Returns:**
  - 200 on success

### POST api/users/login
**Access:** Public \
**Description:** Uses the user's supplied creedentials to perform a login and generate a jwt token \
**Body:** In JSON format
  - Email
  - Password (SHA256 value)

**Returns:**
  - 200 and token on success authenticating user
  - 400 on bad request due to missing values
  - 404 on user not found
  - 500 on server error

### POST api/users/register
**Access:** Public \
**Description:** Used by users to create a new account \
**Body:** In JSON format
  - Email (String)
  - Name (String)
  - Password (SHA256 String)
  - Password Confirmation (SHA256 String)

**Returns:**
  - 200 and token on success creating a new user
  - 400 on bad request due to missing values
  - 500 on server error

### GET api/values
**Access:** Private \
**Description:** Gets all the user's values \
**Returns:**
  - 200 and success response
  - 401 on failed auth
  - 500 on server error

### POST api/values
**Access:** Private \
**Description:** Create a new user value \
**Body:** In JSON format
  - Value Name (String)
  - Value Data (String)
  
**Returns:**
  - 200 and success response
  - 400 on bad request due to missing values
  - 401 on failed auth
  - 500 on server error

### GET api/values/:id/info
**Access:** Private \
**Description:** Get \
**Returns:**
  - 200 and value's information in json format
  - 400 on bad request due to missing values
  - 401 on failed auth
  - 404 if the value wasn't found
  - 500 on server error

### PUT api/values/:id/update
**Access:** Private \
**Description:** Updates an existing user value \
**Body:** In JSON format
  - Value Name (String)
  - Value Data (String)

**Returns:**
  - 200 and success response
  - 400 on bad request due to missing values
  - 401 on failed auth
  - 404 if the value wasn't found
  - 500 on server error

### DELETE api/values/:id/remove
**Access:** Private \
**Description:** Delete an existing user value \
**Returns:**
  - 200 and success response
  - 400 on bad request due to missing values
  - 401 on failed auth
  - 404 if the value wasn't found
  - 500 on server error