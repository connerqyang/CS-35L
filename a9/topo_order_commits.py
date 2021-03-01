#Import necessary libraries
import os, sys, zlib

def topo_order_commits():
    #Find .git folder
    found_git = False
    path_to_git = ''
    
    while not found_git:
        #Search current directory for .git folder
        current_dir = os.getcwd()
        #Check if current directory is .git
        if os.path.isdir(current_dir + "/.git"):
            found_git = True
            path_to_git = current_dir + "/.git"
        if current_dir == '/':
            sys.exit('Not inside a Git respository')
        for root, dirs, files in os.walk(current_dir):
            for name in files:
                if name == '.git':
                    found_git = True
                    path_to_git = root + '/' + str(file)
        #Otherwise, go to parent directory
        if not found_git:
            current_dir = os.path.abspath(os.path.join(current_dir, os.pardir))

    current_dir = path_to_git + '/refs/heads'
    local_branches = get_branches(current_dir, path_to_git)
    #print(local_branches)
    for i in range(0, len(local_branches)):
        temp = local_branches[i] = local_branches[i][temp:]

    local_branches.sort()
    nodes = dict()
    objects = path_to_git + "/objects"

    for branch in local_branches:
        file = open(path_to_git + "/refs/heads/" + branch, "r")
        commit = file.read()
        file.close()
        #build_commit_graph1(path_to_git, objects, nodes, None, commit):

def get_branches(current_dir, path_to_git):
    #Get a list of local branch names                                                  
    current_dir = path_to_git + '/refs/heads'
    local_branches = list()

    for root, dirs, files in os.walk(current_dir):
        for name in files:
            child_file = os.path.join(current_dir, name)
            if os.path.isfile(child_file):
                local_branches.append(child_file)
        for subdir in dirs:
            for root, dirs, files in os.walk(current_dir):
                for name in files:
                    child_file = os.path.join(current_dir, name)
                    if os.path.isfile(child_file):
                        local_branches.append(child_file)
            for subdir in dirs:
                for root, dirs, files in os.walk(current_dir):
                    for name in files:
                        child_file = os.path.join(current_dir, name)
                        if os.path.isfile(child_file):
                            local_branches.append(child_file)

    return local_branches

# Build the commit graph (3)
class CommitNode:
    def __init__(self, commit_hash):
        """
        :type commit_hash: str
        """
        self.commit_hash = commit_hash
        self.parents = set()
        self.children = set()
        self.visited = 0


#def create_nodes(path_to_git, git_objects, nodes, parent):
#    node = CommitNode(parent)

    


topo_order_commits()
