import networkx as nx
from random import choice

def random_vertex(G):
    # return random vertex in graph
    return choice(list(G.nodes()))

def check_coloring_valid(G, C, v, c):
    # look at all of v's neighbors
    for neighbor in G[v]:
        # is v already colored with c?
        if neighbor in C and C[neighbor] == c:
            return False
    return True

# update a k-coloring to a random neighbor in-place
def sample_adjacent_coloring(G, C, k):
    v = random_vertex(G)

    # is the vertex already colored?
    if v in C:
        del C[v]
    else:
        # pick a random color
        new_color = choice(list(range(k)))
        # if the new coloring is OK, update it, else do nothing
        if check_coloring_valid(G, C, v, new_color):
            C[v] = new_color

# sample a k-coloring
def sample_coloring(G, k, steps=1000, initial_C=None):
    C = initial_C if initial_C else {}

    for _ in range(steps):
        sample_adjacent_coloring(G, C, k)

    return C


# class GraphColorer:
# 	def __init__(self, G):
#         # graph G, maybe set of colorings C?
#         self.G = G

# 	def random_color(self, C):
#         # return random color in C
#         return choice(list(C))

#     def is_coloring_valid(self, G, v, c, C):
#         pass

#     def colorVertex(self, vertex, c):
#         # assign vertex color c
#         pass

#     def sampleColor(self, G, k, t):
#         pass
        # graph G (undirected, unweighted)
        # k : # of colors
        # t : # of steps

        # C <- empty coloring
        # for F in range(t):
            # v <- random vertex in G
            # c <- random colors
            # if you take C and then change it
            # by assigning vertex v to have color c
