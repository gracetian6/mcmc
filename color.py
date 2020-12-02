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

# update a k-coloring to a random neighbor in-place - implicit
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

# update a k-coloring to all neighbors and return colorings of neighbors
# explicit version
def list_adjacent_coloring(G, C, k):
	pass
	# self loop
	# return list of non-self loop neighbors (sampled colorings are valid)
	# tells you how to color each vertex - uncolor it, or change

# sample a k-coloring
def sample_coloring(G, k, steps=1000, initial_C=None):
    C = initial_C if initial_C else {}

    for _ in range(steps):
        sample_adjacent_coloring(G, C, k)

    return C
