#include <iostream>

#include "RaftFollowerState.h"
#include "JsonTools.h"

RaftFollowerState::RaftFollowerState(boost::asio::io_service& ios,
                                       Storage& s,
                                       CommandFactory& cf,
                                       ApiCommandQueue& pq,
                                       PeerList& ps)
        : RaftState(ios, s, cf, pq, ps),
          election_timeout_timer_(ios_,
                           boost::posix_time::milliseconds(raft_election_timeout_interval_min_milliseconds))
{
    std::cout << "I am Follower" << std::endl;

    election_timeout_timer_.async_wait(boost::bind(&RaftFollowerState::start_election, this));
}

string RaftFollowerState::handle_request(const string& req)
{
    auto pt = pt_from_json_string(req);

    unique_ptr<Command> command = command_factory_.get_command(pt);
    // If heartbeat command received reset timer.
    string response = pt_to_json_string(command->operator()());

    return response;
}

void RaftFollowerState::start_election()
{

    // Change state to Candidate.
/*
 * If node haven't heard from leader it can start election.
 * Change state to State::candidate and request votes
*/
// Election_Timeout -> time to wait before starting new election (become a candidate)
// Random in 150-300 ms interval

// After Election_Timeout follower becomes candidate and start election term, votes for itself and sends
// Request_Vote messages to other nodes.
// If node hasn't voted for itself or didn't reply to others node Request_Vote it votes "YES" otherwise "NO"
// An resets election timeout (won't start new election).
// When candidate received majority of votes it sets itself as leader.
// The leader sends Append_Entry messages to followers in Heartbeat_Timeout intervals. Followers respond
// If follower don't receive Append_Entry in time alotted new election term starts.
// Handle Split_Vote
}