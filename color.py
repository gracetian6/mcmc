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

def list_adjacent_colorings(G, C, k):
    colorings = []
    self_loop_prob = 0

    for v in G.nodes():
        if v in C:
            # copy the coloring and uncolor v
            new_coloring = C.copy()
            del new_coloring[v]
            # add the edge w.p. 1/n where n = # of vertices in G
            colorings.append((new_coloring, 1/len(G.nodes())))
        else:
            for c in range(k):
                if check_coloring_valid(G, C, v, c):
                    # copy the coloring and set v's color to c
                    new_coloring = C.copy()
                    new_coloring[v] = c
                    # add the edge w.p. 1/kn 
                    colorings.append((new_coloring, 1/(len(G.nodes()) * k)))
                else:
                    self_loop_prob += 1/(len(G.nodes()) * k)

    colorings.append((C, self_loop_prob))
    return colorings

# sample a k-coloring
def sample_coloring(G, k, steps=1000, initial_C=None):
    C = initial_C if initial_C else {}

    for _ in range(steps):
        sample_adjacent_coloring(G, C, k)

    return C

def hash_coloring(C):
    return hash(frozenset(C.items()))

def construct_graph_of_colorings(G, k):
    H = nx.MultiDiGraph()
    queue = [{}]
    x = 0

    while queue:
        C = queue.pop()
        x+=1
        if x%1000==0: print(x)

        for neighbor, weight in list_adjacent_colorings(G, C, k):
            if hash_coloring(neighbor) not in H:
                queue.append(neighbor)

            H.add_edge(hash_coloring(C), hash_coloring(neighbor), weight=weight)

    return H
