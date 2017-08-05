function update_map(response){
  if('map' in response){
    console.log('map');
    let nodes = [];
    let edges = [];
    let lambdas = [];
    response['map']['sites'].forEach(function(node){
      nodes.push(node['id']);
    });
    response['map']['rivers'].forEach(function(river){
      let edge = []
      edge.push(river['source']);
      edge.push(river['target']);
      edges.push(edge);
    });
    response['map']['mines'].forEach(function(mine){
      lambdas.push(mine)
    });
    console.log(nodes);
    console.log(edges);
    console.log(lambdas);
        //Visualizer.update(vcell,data)
  }
  if('claim' in response){
    console.log('claim');
  }
  if('pass' in response){
    console.log('pass');
  }
  if('stop' in response){
    console.log('stop');
  }
  return "end"
}

function updateGame(gameState){
  console.log(gameState);
  let url = 'log/'+String(gameState.gamenumber)+'/'+String(gameState.turn);
  let vcell = $('#main-visualize-cell');
  let jsonlog = $('#jsonlog')
  $.ajax({
    type:"GET",
    dataType:"json",
    url:url
    }
  ).done(function(response,status,jqXHR){
    let data = {};
    jsonlog.val(jqXHR['responseText'])
    data = update_map(response)
    gameState.turn++;
  }).fail(function(response,status,error){
    jsonlog.val(error)
    alert("The game is over!! or some error has occur.")
  });
}

var game = {}
var timer = {}

$('#play').on('click',function(){
  game = {gamenumber:Number($('#gamenumber').val()),turn:0}
  updateGame(game);
});

$('#next').on('click',function(){
  updateGame(game);
});

$('#autoplay').on('click',function(){
  timer = setInterval('updateGame(game);',1000);
});

$('#stopplay').on('click',function(){
  clearInterval(timer);
});
