# test.py
import networkx as nx
# from random import choice
import color
import matplotlib.pyplot as plt

# # pick an interesting small graph to try
# G = nx.tutte_graph()
# random_coloring = color.sample_coloring(G, 5)
# print(random_coloring)
#
# list_of_colors = [(random_coloring[x] if x in random_coloring else 0) for x in range(G.number_of_nodes())]
#
# nx.draw(G, node_color=list_of_colors)
# plt.show()


# Path graph
path = nx.path_graph(8)
random_coloring = color.sample_coloring(path, 10)
list_of_colors = [(random_coloring[x] if x in random_coloring else 0) for x in range(path.number_of_nodes())]
nx.draw(path, node_color=list_of_colors)
plt.show()

# build graph of colorings 
