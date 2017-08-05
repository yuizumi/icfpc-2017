// 1. vis = new Visulize();
// 2. vis.createMap('#main-visualize-cell', data)
// 3. visualze.update(src_idx, dst_idx)

// data = {};
// data.nodes = [node_id, ...]
// data.edges = [[src_id, dst_id], ...]
// data.lambdas = [node_idx ...]

var Visualizer = {};

Visualizer.createMap = function (root_selector, data) {
    console.log('selector', root_selector);
    console.log('map_data:', data);
    return 0;
};

Visualizer.update = function (src_node_id, dst_node_id) {
    console.log(src_node_id + '->' + dst_node_id);
    return 0;
};