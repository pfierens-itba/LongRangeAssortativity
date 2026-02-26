# -*- coding: utf-8 -*-

import json
import networkx as nx
import numpy as np

#%%

def parse_deezer(file1,file2,file3):
    G       = nx.read_edgelist(file1, delimiter=',',nodetype=int)
    N       = G.order()
    featlo  = np.zeros(N,dtype=np.uint64) 
    feathi  = np.zeros(N,dtype=np.uint32) 
    try:
        # Open the file in read mode ('r')
        f1 = open(file3, 'wb')
        with open(file2, 'r') as file:
            # Deserialize the file data into a Python object (e.g., a dictionary)
            data = json.load(file)
            s = set()
            for n in range(N):
                s = s.union(data[str(n)])
            for n in range(N):
                nfeat   = set(data[str(n)])
                value   = np.uint64(0)
                bit     = np.uint64(1)
                k       = 1
                for item in s:
                    if item in nfeat:
                        value = value | bit
                        # if n == 0:
                        #     print(k)
                    bit = bit << 1
                    k   = k + 1
                    if k > 64:
                        valuelo     = np.uint64(value)
                        featlo[n]   = value
                        value       = np.uint32(0)
                        bit         = np.uint32(1)
                        k           = 1
                        
                feathi[n] = value
                f1.write(valuelo.tobytes())
                f1.write(value.tobytes())
            
            feat = [featlo,feathi]
            f1.close()
    except FileNotFoundError:
        print("Error: The file 'data.json' was not found.")
    except json.JSONDecodeError as e:
        print(f"Error: Failed to decode JSON from the file: {e}")
        
    return G, feat
G, feat = parse_deezer("HR_edges.csv","HR_genres.json","deezer-feat-HR.bin")
G, feat = parse_deezer("HU_edges.csv","HU_genres.json","deezer-feat-HU.bin")
G, feat = parse_deezer("RO_edges.csv","RO_genres.json","deezer-feat-RO.bin")


#%%

#It is essentially a copy from  https://github.com/jcatw/scnn/blob/master/scnn/data.py,
#visited on 2026-02-26. The original code is licensed under MIT License Copyright (c) 2016 James Atwood
def parse_pubmed(): 
    n_nodes = 19717
    n_features = 500
    n_clastes = 3

    G = nx.Graph()
    G.add_nodes_from(np.arange(19717))
    
    data_X = np.zeros((n_nodes, n_features), dtype='float')
    data_Y = np.zeros((n_nodes, n_clastes), dtype='int32')
 
    paper_to_index = {}
    feature_to_index = {}
    index_to_paper = {}
    
    # parse nodes
    with open('Pubmed-Diabetes.NODE.paper.tab','r') as node_file:
        # first two lines are headers
        node_file.readline()
        node_file.readline()
 
        k = 0

        # for i,line in enumerate(node_file.readlines()):
        for i,line in enumerate(node_file.readlines()):
            items = line.strip().split('\t')
 
            paper_id = items[0]
            paper_to_index[paper_id] = i
            index_to_paper[i] = paper_id
            
            # label=[1,2,3]
            label = int(items[1].split('=')[-1]) - 1  # subtract 1 to zero-count
            data_Y[i,label] = 1.
 
            # f1=val1 \t f2=val2 \t ... \t fn=valn summary=...
            features = items[2:-1]
            for feature in features:
                parts = feature.split('=')
                fname = parts[0]
                fvalue = float(parts[1])
 
                if fname not in feature_to_index:
                    feature_to_index[fname] = k
                    k += 1
 
                data_X[i, feature_to_index[fname]] = fvalue
 
            G.nodes[i]['features'] = data_X[i,:]
            
    # parse graph
    # data_A = np.zeros((n_nodes, n_nodes), dtype='float')    
    diff = 0
    with open('Pubmed-Diabetes.DIRECTED.cites.tab','r') as edge_file:
        # first two lines are headers
        edge_file.readline()
        edge_file.readline()
 
        # for i,line in enumerate(edge_file.xreadlines()):
        for i,line in enumerate(edge_file.readlines()):
 
            # edge_id \t paper:tail \t | \t paper:head
            items = line.strip().split('\t')
 
            # edge_id = items[0]
 
            tail = items[1].split(':')[-1]
            head = items[3].split(':')[-1]
 
            # data_A[paper_to_index[tail],paper_to_index[head]] = 1.0
            # data_A[paper_to_index[head],paper_to_index[tail]] = 1.0
            if paper_to_index[tail] != paper_to_index[head]:
                G.add_edge(paper_to_index[tail],paper_to_index[head])
            
            if i - nx.number_of_edges(G) != diff:                
                print(i,nx.number_of_edges(G),tail,head)
                diff = i - nx.number_of_edges(G)
                
    return G
    

G = parse_pubmed()
        
nx.write_edgelist(G, 'pubmed_edges.txt', data = False)
with open('pubmed_feat.bin', 'wb') as f:
    for n in G.nodes:
        ff = G.nodes[n]['features']
        for k in range(500):
            f.write(ff[k])

#%%

#It is essentially a copy from  https://github.com/jcatw/scnn/blob/master/scnn/data.py,
#visited on 2026-02-26. The original code is licensed under MIT License Copyright (c) 2016 James Atwood.   
def parse_cora(plot=False):
    id2index = {}
 
    label2index = {
        'Case_Based': 0,
        'Genetic_Algorithms': 1,
        'Neural_Networks': 2,
        'Probabilistic_Methods': 3,
        'Reinforcement_Learning': 4,
        'Rule_Learning': 5,
        'Theory': 6
    }

    G = nx.Graph()
    features = []
    labels = []
 
    with open('cora.content', 'r') as f:
        i = 0
        # for line in f.xreadlines():
        for line in f.readlines():
            items = line.strip().split('\t')
 
            id = items[0]
 
            # 1-hot encode labels
            label = np.zeros(len(label2index))
            label[label2index[items[-1]]] = 1
            labels.append(label)
 
            # parse features
            features.append([int(x) for x in items[1:-1]])
 
            id2index[id] = i
            #G.add_node(i,feat = np.asarray(features[-1],dtype='float'))
            G.add_node(i,feat = features[-1])
            i += 1
 
    features = np.asarray(features, dtype='float')
    labels = np.asarray(labels, dtype='int32')
 
    # n_papers = len(id2index)
    # adj = np.zeros((n_papers, n_papers), dtype='float32') 
    with open('cora.cites', 'r') as f:
        # for line in f.xreadlines():
        for line in f.readlines():
            items = line.strip().split('\t')
            # adj[ id2index[items[0]], id2index[items[1]] ] = 1.0
            # # undirected
            # adj[ id2index[items[1]], id2index[items[0]] ] = 1.0
            G.add_edge(id2index[items[0]],id2index[items[1]])
    


    return G,labels,features

G,labels,features = parse_cora()


nx.write_edgelist(G, 'cora_edges.txt', data=False)
with open('cora_feat.bin','wb') as f:
    for n in range(len(G.nodes)):
        for k in range(features[n].shape[0]):
            f.write(int(features[n,k]).to_bytes(1))
