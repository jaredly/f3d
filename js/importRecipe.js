const parseRecipeWebsite = require('./parseRecipeWebsite')

const proxy = 'https://galvanize-cors-proxy.herokuapp.com/'

module.exports.importRecipe = url => {
  return fetch(proxy + url, {
    headers: {'Origin': location.origin}
  })
    .then(r => r.text())
    .then(text => parseRecipeWebsite(text))
}