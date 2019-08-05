
const cheerio = require('cheerio')
const {parseIngredient} = require('bs/EditRecipe/PasteUtils.js')

const unwhite = text => {
  return text.replace(/\s+/g, ' ').trim()
}

const getTime = elem => {
  // console.log('got time', elem)
  if (!elem) return null
  const dt = elem.attr('content') || elem.attr('datetime')
  // console.log('dt', dt)
  if (dt) {
    const match = dt.trim().match(/^PT((\d+)H)?((\d+)M)?$/)
    if (match) {
      // console.log(match)
      // console.log(match[2], match[4])
      return +(match[2] || 0) * 60 + +(match[4] || 0)
    }
  }
  let n = parseInt(elem.text())
  if (isNaN(n)) {
    return null
  }
  return n
}

const zz = `
// these are the things from the recipe html metadata thing
"aggregateRating"
"ratingValue"
"ratingCount"
"bestRating"
"worstRating"
"image"

//
"nutrition"
  "calories"
  "carbohydrateContent"
  "cholesterolContent"
  "fiberContent"
  "proteinContent"
  "saturatedFatContent"
  "servingSize"
  "sodiumContent"
`

module.exports = text => {
  const $ = cheerio.load(text)
  const title = unwhite($('[itemprop=name]').text())
  const description = unwhite($('[itemprop=description]').text())
  let ingredients = $('[itemprop=ingredients]')
    .map((i, elem) => $(elem).text())
    .get()
    .map(unwhite)
    .map(parseIngredient)
  if (!ingredients.length) {
    ingredients = $('[itemprop=recipeIngredient]')
      .map((i, elem) => $(elem).text())
      .get()
      .map(unwhite)
      .map(parseIngredient)
  } 

  const allRecipeInstructions = $('[itemprop=recipeInstructions]')
    .map((i, elem) => {
      const dt = $(elem).find('dt').get()
      // console.log('instructions: dt', dt.length)
      if (dt.length) return dt.map(el => $(el).text())
      const li = $(elem).find('li').get()
      // console.log('instructions: li', li.length)
      if (li.length) return li.map(el => $(el).text())
      return [$(elem).text()]
    })
    .get()

  const instructions = [].concat(...allRecipeInstructions).map(unwhite)
    .map(text => ({text, ingredientsUsed: {}}))  
  // TODO nutrition
  const totalTime = getTime($('[itemprop=totalTime]'))
  const prepTime = getTime($('[itemprop=prepTime]'))
  const cookTime = getTime($('[itemprop=cookTime]'))
  let yieldUnit = $('[itemprop=recipeyield]').text()
  if (yieldUnit === '') yieldUnit = null

  return {
    title,
    description,
    ingredients,
    instructions,
    meta: {
      yieldUnit,
      yield: null,

      totalTime,
      prepTime,
      cookTime,
      ovenTemp: null,
    }
  }
}
