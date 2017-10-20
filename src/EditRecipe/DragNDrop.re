
let startMoving ::positions ::key ::margin ::onDrop evt => {
  ReactEventRe.Mouse.preventDefault evt;
  /* Js.log2 "moving" key; */
  [%bs.raw {|function(positions, key, margin, onDrop){
    /* console.log('key', key, positions) */

    var indicator = document.createElement('div')
    document.body.appendChild(indicator)
    indicator.style.backgroundColor = '#aaa'
    indicator.style.height = '4px'
    indicator.style.marginTop = '-2px'
    indicator.style.position = 'fixed'
    indicator.style.pointerEvents = 'none'
    positions[key].style.opacity = 0.5

    var dest = null

    var handler = function(evt) {
      Object.keys(positions).some((k, i, all) => {
        const box = positions[k].getBoundingClientRect()
        const mid = (box.top + box.bottom) / 2
        if (evt.clientY < mid) {
          dest = [box.top - margin, {key: k, after: false}, box.left, box.width]
          return true
        } else if (i === all.length - 1) {
          dest = [box.bottom + margin, {key: k, after: true}, box.left, box.width]
        }
      })
      indicator.style.top = dest[0] + 'px'
      indicator.style.left = dest[2] + 'px'
      indicator.style.width = dest[3] + 'px'
    }

    var destroy = function() {
      indicator.parentNode.removeChild(indicator)
      window.removeEventListener('mousemove', handler)
      window.removeEventListener('mouseup', destroy)
      positions[key].style.opacity = 1
      if (dest && dest[1].key !== key) {
        onDrop([key, dest[1].key, dest[1].after])
      }
    }

    window.addEventListener('mousemove', handler)
    window.addEventListener('mouseup', destroy)
  }|}] positions key margin onDrop [@@bs];
};
