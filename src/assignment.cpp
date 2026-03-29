//
// Created by Daniel on 29/03/2026.
//

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


// adds forward (cap) & backward (0) edges linked by reverse for flow
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

// BFS
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

// edmonds-Karp
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
            if (ds.submissions[i].primaryTopic == ds.reviewers[j].primaryTopic)
                match = ds.submissions[i].primaryTopic;

            if (match != 0)
                addResidualEdge(g, subNode, revNode, 1);
        }
    }

    // edmonds-Karp
    edmondsKarp(g, SOURCE, SINK);

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
            a.matchedDomain = ds.submissions[i].primaryTopic;
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