module.exports = (app) => {
  const NeoLog = require('../structs/NeoLog')
  var oauth = require("./Fortnite-Game")
  const axios = require('axios').default;
	app.get('/fortnite/api/storefront/v2/catalog', function (req, res) {
    NeoLog.Log(`Loaded shop file for ${oauth.authID}`)
    const Userconfig = require(`../config/${oauth.authID}/profiles/profile_uConfig.json`)
    //${Userconfig.shop}
      axios.get(Userconfig.shop, { timeout: 3000 }).then(response => {
        res.json(response.data);
      })
	});
}