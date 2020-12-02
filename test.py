# test.py
import networkx as nx
# from random import choice
import color
import matplotlib.pyplot as plt

# networkx
# G = nx.Graph()
# G.add_edge(1, 2, weight=4)
# G.add_edge(2, 4, weight=2)
# G.add_edge(1, 3, weight=3)
# G.add_edge(3, 4, weight=4)
# nx.shortest_path(G, 1, 4, weight='weight')
# print(nx.shortest_path(G, 1, 4, weight='weight'))

# d = nx.coloring.greedy_color(G, strategy="largest_first")
# print(d)
# # random vertex
# print(choice(list(G.nodes())))

# pick an interesting small graph to try
G = nx.tutte_graph()
random_coloring = color.sample_coloring(G, 5)
print(random_coloring)

list_of_colors = [(random_coloring[x] if x in random_coloring else 0) for x in range(G.number_of_nodes())]

nx.draw(G, node_color=list_of_colors)
plt.show()