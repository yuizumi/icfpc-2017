// data = {};
// data.nodes = [node_id, ...]
// data.edges = [[src_id, dst_id], ...]
// data.lambdas = [node_idx ...]

var Visualizer = function (rootSelector) {
    /*
    描画するrootのセレクタを指定してnewで呼び出す
     */
    this._rootSelector = rootSelector;
    this._mapData = {};

    this.setMapData = function (mapData) {
        this._mapData = mapData;
    };
    this.getMapData = function () {
        return this._mapData;
    };

    console.log('selector', rootSelector);
};


// マップの初期化
Visualizer.prototype.createMap = function (data) {
    console.log('map_data:', data);
    this.setMapData(data);
    return 0;
};

// エッジの更新
Visualizer.prototype.update = function (src_node_id, dst_node_id, user_id) {
    console.log(src_node_id + '->' + dst_node_id);
    return 0;
};


// テスト
var VisualizeTest = function () {
    var vis = new Visualizer('#main-visualize-cell'); 
    
    vis.createMap({
        nodes: [1, 2, 3, 5],
        edged: [[1, 2], [1, 5], [2, 3]],
        lambdas: [1]
    });
    
    vis.update(1, 2, 0);
};
