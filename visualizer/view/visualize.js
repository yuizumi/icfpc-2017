var Visualizer = function (rootSelector) {
    /*
     描画するrootのセレクタを指定してnewで呼び出してください
     */

    this.init = function () {
        this._rootSelector = rootSelector;
        this._mapData = {};
        console.log('selector', rootSelector);
    };

    this.setMapData = function (mapData) {
        this._mapData = mapData;
    };

    this.getMapData = function () {
        return this._mapData;
    };

    // マップの初期化
    this.createMap = function (data) {
        /*
         data: {
         nodes: [1, 2, 3, 5 ...],
         edged: [[1, 2], [1, 5], [2, 3] ...],
         lambdas: [1 ...]
         }
         */

        console.log('map_data:', data);
        this.setMapData(data);
        return 0;
    };


    // エッジの更新
    this.update = function (src_node_id, dst_node_id, user_id) {
        console.log(src_node_id + '->' + dst_node_id + ': ' + user_id);
        return 0;
    };


    //オブジェクトのloadを終えたらinitする
    this.init();
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

VisualizeTest();