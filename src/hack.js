
const data = require('./dump').data
const init = () => {
  console.log(data)
  const db = fb.firestore()
  const uid = fb.auth().currentUser.uid
  const prom = [
    /*
    db.collection('userData').doc(uid).set({
      flickrToken: null,
      following: [],
      homepageLists: [],
    }).catch(err => console.log(err, 'userdata')),
    // */

    /*
    ...data.ingredients.map(ingredient => {
      ingredient.authorId = uid
      ingredient.alternativeNames = []
      ingredient.diets = []
      ingredient.calories = null
      return db.collection('ingredients').doc(ingredient.id).set(ingredient)
      .catch(err => console.log(err, ingredient))
    }),
    // */

    //*
    ...data.recipes.map(recipe => {
      recipe.authorId = uid
      recipe.collaborators = {}
      recipe.isPrivate = false
      recipe.tags = {}
      recipe.instructions.forEach(i => i.ingredientsUsed = {})
      recipe.ingredientsUsed = {}
      recipe.ingredients.forEach(ing => recipe.ingredientsUsed[ing.id] = true)
      recipe.meta = {
        yield: recipe.yield,
        yieldUnit: recipe.yieldUnit,
        cookTime: recipe.cookTime,
        prepTime: recipe.prepTime,
        totalTime: recipe.totalTime,
        ovenTemp: recipe.ovenTemp,
      }
      delete recipe.yield
      delete recipe.yieldUnit
      delete recipe.cookTime
      delete recipe.prepTime
      delete recipe.totalTime
      delete recipe.ovenTemp

      const stopwords = require('stopword/lib/stopwords_en.js').words
      const stopMap = stopwords.reduce((m, w) => (m[w] = true, m), {})
      const tokenRegex = /[^a-zа-яё0-9\-']+/i;

      recipe.titleSearch = {}

      recipe.title.toLowerCase().split(tokenRegex)
      .filter(token => token && !stopMap[token])
      .forEach(token => {
        recipe.titleSearch[token] = true
      })

      recipe.comments = []

      delete recipe.author
      return db.collection('recipes').doc(recipe.id).set(recipe)
      .catch(err => console.log(err, recipe))
    }),
    // */

    /*
    ...data.home.user.lists.map(list => {
      list.authorId = uid
      delete list.author
      list.collaborators = {}
      list.isPrivate = false
      list.recipes = list.recipes.reduce((rs, r) => (rs[r.id] = true, rs), {})
      return db.collection('lists').doc(list.id).set(list)
      .catch(err => console.log(err, list))
    })
    // */
  ]
  Promise.all(prom).then(() => console.log('done!'), err => console.error(err))
}
window.hack = init;
