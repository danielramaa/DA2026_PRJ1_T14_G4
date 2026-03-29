/**
 * @file assignment.cpp
 * @brief Assignment, max-flow, output writing, and risk analysis implementation.
 */

#include "assignment.h"
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <fstream>

// node IDs
static const int SOURCE   = 0;
static const int SINK     = 1;
static const int SUB_BASE = 2000;
static const int REV_BASE = 4000;


/**
 * @brief Adds a forward and reverse edge pair to build residual-capacity flow graph links.
 * @param g Flow graph to update.
 * @param from Origin node ID.
 * @param to Destination node ID.
 * @param cap Capacity of the forward edge.
 * @complexity O(V), due to vertex lookup/creation operations in the underlying graph structure.
 */
static void addResidualEdge(Graph<int>& g, int from, int to, double cap) {
    g.addVertex(from);
    g.addVertex(to);
    const auto vFrom = g.findVertex(from);
    const auto vTo   = g.findVertex(to);
    auto fwd = vFrom->addEdge(vTo, cap);
    auto bwd = vTo->addEdge(vFrom, 0);
    fwd->setReverse(bwd);
    bwd->setReverse(fwd);
    fwd->setFlow(0);
    bwd->setFlow(0);
}

/**
 * @brief Runs breadth-first search on the residual graph to find an augmenting path.
 * @param g Residual graph.
 * @param s Source node ID.
 * @param t Sink node ID.
 * @param parent Map populated with predecessor edges for path reconstruction.
 * @return True if an augmenting path from source to sink is found, false otherwise.
 * @complexity O(V + E), where V is number of vertices and E is number of edges.
 */
static bool bfs(Graph<int>& g, int s, int t, std::map<int, Edge<int>*>& parent) {
    for (auto v : g.getVertexSet())
        v->setVisited(false);
    parent.clear();

    auto src = g.findVertex(s);
    if (!src) return false;
    src->setVisited(true);

    std::queue<int> q;
    q.push(s);

    while (!q.empty()) {
        int cur = q.front(); q.pop();
        auto vCur = g.findVertex(cur);

        for (auto e : vCur->getAdj()) {
            int nxt = e->getDest()->getInfo();
            double residual = e->getWeight() - e->getFlow();
            if (!e->getDest()->isVisited() && residual > 1e-9) {
                e->getDest()->setVisited(true);
                parent[nxt] = e;
                if (nxt == t) return true;
                q.push(nxt);
            }
        }
    }
    return false;
}

/**
 * @brief Computes maximum flow from source to sink using the Edmonds-Karp algorithm.
 * @param g Residual graph containing capacities.
 * @param s Source node ID.
 * @param t Sink node ID.
 * @return Maximum flow value pushed from source to sink.
 * @complexity O(VE^2), where V is number of vertices and E is number of edges.
 */
static double edmondsKarp(Graph<int>& g, int s, int t) {
    for (auto v : g.getVertexSet())
        for (auto e : v->getAdj())
            e->setFlow(0);

    double totalFlow = 0;
    std::map<int, Edge<int>*> parent;

    while (bfs(g, s, t, parent)) {
        // bottleneck
        double pushed = INF;
        int cur = t;
        while (cur != s) {
            auto e = parent[cur];
            pushed = std::min(pushed, e->getWeight() - e->getFlow());
            cur = e->getOrig()->getInfo();
        }

        // augment flow along the path
        cur = t;
        while (cur != s) {
            auto e = parent[cur];
            e->setFlow(e->getFlow() + pushed);
            e->getReverse()->setFlow(e->getReverse()->getFlow() - pushed);
            cur = e->getOrig()->getInfo();
        }

        totalFlow += pushed;
    }
    return totalFlow;
}

/**
 * @brief Builds and solves the reviewer-assignment flow model for the provided dataset.
 * @param ds Parsed dataset with submissions, reviewers, and assignment controls.
 * @param mode Matching mode controlling which topic matches are allowed.
 * @return AssignmentResult containing produced assignments and unmet review requirements.
 * @complexity O(VE^2 + S*R), dominated by max-flow; S is submissions and R is reviewers.
 */
AssignmentResult runAssignment(const Dataset& ds, int mode) {
    AssignmentResult result;
    result.success = true;

    Graph<int> g;
    int minR = ds.params.minReviewsPerSubmission;
    int maxR = ds.params.maxReviewsPerReviewer;

    g.addVertex(SOURCE);
    g.addVertex(SINK);

    // source → Submission (capacity = minReviewsPerSubmission)
    for (int i = 0; i < (int)ds.submissions.size(); i++) {
        int subNode = SUB_BASE + i;
        addResidualEdge(g, SOURCE, subNode, minR);
    }

    // reviewer → Sink (capacity = maxReviewsPerReviewer)
    for (int j = 0; j < (int)ds.reviewers.size(); j++) {
        int revNode = REV_BASE + j;
        addResidualEdge(g, revNode, SINK, maxR);
    }

    // submission → Reviewer (capacity = 1, only if topics match)
    for (int i = 0; i < (int)ds.submissions.size(); i++) {
        for (int j = 0; j < (int)ds.reviewers.size(); j++) {
            int subNode = SUB_BASE + i;
            int revNode = REV_BASE + j;
            int match = 0;

            // mode 1: only primary topics
            if (mode == 1) {
                if (ds.submissions[i].primaryTopic == ds.reviewers[j].primaryTopic)
                    match = ds.submissions[i].primaryTopic;
            }
            else if (mode == 2) {
                if (ds.submissions[i].primaryTopic == ds.reviewers[j].primaryTopic)
                    match = ds.submissions[i].primaryTopic;
                else if (ds.submissions[i].secondaryTopic != 0 &&
                         ds.submissions[i].secondaryTopic == ds.reviewers[j].primaryTopic)
                    match = ds.submissions[i].secondaryTopic;
            }

            if (match != 0)
                addResidualEdge(g, subNode, revNode, 1);
        }
    }

    // edmonds-Karp
    edmondsKarp(g, SOURCE, SINK);

    // store the matched topic for each (submission node, reviewer node) pair
    std::map<std::pair<int, int>, int> matchMap;
    for (int i = 0; i < (int)ds.submissions.size(); i++) {
        for (int j = 0; j < (int)ds.reviewers.size(); j++) {
            int subNode = SUB_BASE + i;
            int revNode = REV_BASE + j;
            int match = 0;

            if (mode == 1) {
                if (ds.submissions[i].primaryTopic == ds.reviewers[j].primaryTopic)
                    match = ds.submissions[i].primaryTopic;
            }
            else if (mode == 2) {
                if (ds.submissions[i].primaryTopic == ds.reviewers[j].primaryTopic)
                    match = ds.submissions[i].primaryTopic;
                else if (ds.submissions[i].secondaryTopic != 0 &&
                         ds.submissions[i].secondaryTopic == ds.reviewers[j].primaryTopic)
                    match = ds.submissions[i].secondaryTopic;
            }

            if (match != 0)
                matchMap[{subNode, revNode}] = match;
        }
    }

    // extract assignments with flow > 0
    for (int i = 0; i < (int)ds.submissions.size(); i++) {
        int subNode = SUB_BASE + i;
        auto vSub = g.findVertex(subNode);
        if (!vSub) continue;

        for (auto e : vSub->getAdj()) {
            int destInfo = e->getDest()->getInfo();
            if (destInfo < REV_BASE) continue; // not a reviewer
            if (e->getFlow() < 0.5) continue;  // no flow

            int j = destInfo - REV_BASE;
            Assignment a;
            a.submissionId  = ds.submissions[i].id;
            a.reviewerId    = ds.reviewers[j].id;
            a.matchedDomain = matchMap[{subNode, destInfo}];
            result.assignments.push_back(a);
        }
    }

    // check if all submissions have enough reviews
    for (int i = 0; i < (int)ds.submissions.size(); i++) {
        int subNode = SUB_BASE + i;
        auto vSub = g.findVertex(subNode);
        int count = 0;
        for (auto e : vSub->getAdj())
            if (e->getDest()->getInfo() >= REV_BASE && e->getFlow() > 0.5)
                count++;
        if (count < minR){
            result.success = false;
            int missing = minR - count;
            int domain  = ds.submissions[i].primaryTopic;
            result.missing.push_back({ds.submissions[i].id, domain, missing});
        }
    }

    // sort assignments
    std::sort(result.assignments.begin(), result.assignments.end(),
        [](const Assignment& a, const Assignment& b) {
            if (a.submissionId != b.submissionId) return a.submissionId < b.submissionId;
            return a.reviewerId < b.reviewerId;
        });

    return result;
}

/**
 * @brief Writes assignment results and optional risk-analysis details to a CSV-like output file.
 * @param filename Output file path.
 * @param result Assignment result to export.
 * @param riskyReviewers List of risky reviewer IDs.
 * @param riskK Risk-analysis flag/value that controls risk section emission.
 * @complexity O(A log R + M + K), where A is assignments, R is reviewers present in assignment map, M is missing entries, and K is risky reviewers.
 */
void writeOutput(const std::string& filename,
                 const AssignmentResult& result,
                 const std::vector<int>& riskyReviewers,
                 int riskK)
{
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error: cannot open output file: " << filename << "\n";
        return;
    }

    // assignments
    out << "#SubmissionId,ReviewerId,Match\n";
    for (auto& a : result.assignments)
        out << a.submissionId << ", " << a.reviewerId << ", " << a.matchedDomain << "\n";

    // dual view: reviewer → submissions
    out << "#ReviewerId,SubmissionId,Match\n";
    std::map<int, std::vector<const Assignment*>> revMap;
    for (auto& a : result.assignments)
        revMap[a.reviewerId].push_back(&a);
    for (auto& [revId, list] : revMap) {
        for (auto* a : list)
            out << a->reviewerId << ", " << a->submissionId << ", " << a->matchedDomain << "\n";
    }

    out << "#Total: " << result.assignments.size() << "\n";

    // missing reviews in submissons
    if (!result.missing.empty()) {
        out << "#SubmissionId,Domain,MissingReviews\n";
        for (auto& [subId, domain, count] : result.missing)
            out << subId << ", " << domain << ", " << count << "\n";
    }

    // risk analysis
    if (riskK > 0) {
        out << "#Risk Analysis: " << riskK << "\n";
        for (size_t i = 0; i < riskyReviewers.size(); i++) {
            if (i > 0) out << ", ";
            out << riskyReviewers[i];
        }
        out << "\n";
    }

    out.close();
}

/**
 * @brief Finds reviewers whose removal makes the assignment problem infeasible.
 * @param ds Baseline dataset used for leave-one-reviewer-out simulation.
 * @return Sorted reviewer IDs classified as risky.
 * @complexity O(R * T), where R is number of reviewers and T is one runAssignment execution.
 */
std::vector<int> runRiskAnalysis(const Dataset& ds) {
    std::vector<int> riskyReviewers;
    int mode = ds.control.generateAssignments;

    for (size_t j = 0; j < ds.reviewers.size(); j++) {
        // Temp dataset without reviewer j
        Dataset temp = ds;
        temp.reviewers.erase(temp.reviewers.begin() + j);

        // Run without reviewer
        AssignmentResult result = runAssignment(temp, mode);

        // if failed, critical reviewer
        if (!result.success)
            riskyReviewers.push_back(ds.reviewers[j].id);
    }

    std::sort(riskyReviewers.begin(), riskyReviewers.end());
    return riskyReviewers;
}