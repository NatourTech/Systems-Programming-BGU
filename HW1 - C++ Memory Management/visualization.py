import sys
import networkx as nx
import matplotlib.pyplot as plt
import json


def state_to_color(state):
    if state == 'Waiting':
        return 'Green'
    elif state == 'CollectingOffers':
        return 'Orange'
    elif state == 'Joined':
        return 'Red'

def draw_graph(G, i):
    pos = nx.circular_layout(G)
    nodes_data = G.nodes(data=True)
    colors = [state_to_color(data['state']) for _, data in nodes_data]
    nodes_fig = nx.draw_networkx_nodes(G, pos, node_size=20000, node_color=colors)
    nx.draw_networkx_labels(G, pos, font_size=12,  
                            labels={node[0]: f'id={node[0]}\nname={node[1]["name"]}\nmandates={node[1]["mandates"]}\nstate={node[1]["state"]}\ncoalition={node[1]["coalition"]}' for node in nodes_data})

    nx.draw_networkx_edges(G, pos, width=3, edgelist=G.edges)
    edge_labels = nx.get_edge_attributes(G, "weight")
    nx.draw_networkx_edge_labels(G, pos, edge_labels, label_pos=0.45, font_size=16)
    ax = plt.gca()
    ax.margins(0.1)
    ax.collections[0].set_edgecolor("#000000") 
    ax.collections[0].set_linewidth(2)
    ax.figure.set_size_inches(10, 10)
    if i == 0:
        ax.set_title(f'Initial state')
    else:
        ax.set_title(f'Iteration {i - 1}')
    plt.axis('off')
    plt.tight_layout()
    plt.show()


def get_coalition(partyId, coalitions):
    for i, coalition in enumerate(coalitions):
        if partyId in coalition['parties']:
            return i
    return 'None'

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python3 visualization.py <file_path>")
        exit(1)
    file_path = sys.argv[1]
    with open(file_path, 'r') as f:
        data = json.load(f)

    for i, simulation_state in enumerate(data):
        G = nx.Graph()
        for party in simulation_state['parties']:
            G.add_node(party['id'], mandates=party['mandates'], state=party['state'], 
                        name=party['name'], coalition=get_coalition(party['id'], simulation_state['coalitions']))
        for edge in simulation_state['edges']:
            G.add_edge(edge['from'], edge['to'], weight=edge['weight'])
        draw_graph(G, i)



