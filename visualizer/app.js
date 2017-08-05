function update_map(vcell,response){
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
    //console.log(nodes);
    //console.log(edges);
    //console.log(lambdas);
    data = {"nodes":nodes,"edges":edges,"lambdas":lambdas};
    Visualizer.createMap(vcell,data)
  }
  if('claim' in response){
    let punter = response['claim']['punter'];
    let s = response['claim']['source'];
    let t = response['claim']['target'];
    Visualizer.update(s,t);
    //console.log('claim')
  }
  if('pass' in response){
    console.log('pass');
  }
  if('stop' in response){
    //console.log('stop');
    response['stop']['scores'].forEach(function(score){
      console.log(score);
    });
  }
  return "end"
}

function updateGame(vcell,gameState){
  console.log(gameState);
  let url = 'log/'+String(gameState.gamenumber)+'/'+String(gameState.turn);
  //let vcell = $('#main-visualize-cell');
  let jsonlog = $('#jsonlog')
  $.ajax({
    type:"GET",
    dataType:"json",
    url:url
    }
  ).done(function(response,status,jqXHR){
    let data = {};
    jsonlog.val(jqXHR['responseText'])
    data = update_map(vcell,response)
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
  updateGame('#main-visualize-cell',game);
});

$('#next').on('click',function(){
  updateGame('#main-visualize-cell',game);
});

$('#autoplay').on('click',function(){
  timer = setInterval('updateGame("#main-visualize-cell",game);',1000);
});

$('#stopplay').on('click',function(){
  clearInterval(timer);
});
