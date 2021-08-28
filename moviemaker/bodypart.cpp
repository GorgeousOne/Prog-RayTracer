#include "bodypart.h"

std::ostream &operator<<(std::ostream &os, BodyPart const& part) {
	for (auto const& child : part.children) {
		os << *child;
	}
	os << "define shape obj " << part.name << std::endl;

	if (!part.children.empty()) {
		os << "add " << part.name;

		for (auto const &child: part.children) {
			os << " " << child->name;
		}
		os << std::endl;
	}
	os << "transform " << part.name << " translate " << part.offset.x << " " << part.offset.y << " " << part.offset.z << std::endl;
	os << "transform " << part.name << " rotate " << part.rotation.x << " " << part.rotation.y << " " << part.rotation.z << std::endl;
	os << std::endl;
	return os;
}

Body::Body() : offset_() {
	auto body = std::make_shared<BodyPart>(BodyPart{"chest", {0, 97.4361, 0}});
	auto head = std::make_shared<BodyPart>(BodyPart{"head", {-2.6547f, 63.9209f, 2.93762f}});
	auto arm_left = std::make_shared<BodyPart>(BodyPart{"arm-left", {-25.1092f, 48.3269f, 3.80979f}});
	auto arm_right = std::make_shared<BodyPart>(BodyPart{"arm-right", {16.7486, 50.3349, 5.91147}});
	auto hand_left = std::make_shared<BodyPart>(BodyPart{"hand-left", {-2.2336, 1.75708, 23.991}});
	auto hand_right = std::make_shared<BodyPart>(BodyPart{"hand-right", {3.19166, 2.45541, 23.599}});
	auto leg_left = std::make_shared<BodyPart>(BodyPart{"leg-left", {-6.3, 0, 0}});
	auto leg_right = std::make_shared<BodyPart>(BodyPart{"leg-right", {6.3, 0, 0}});
	auto foot_left = std::make_shared<BodyPart>(BodyPart{"foot-left", {-0.91754, 0.8892, 43.1269}});
	auto foot_right = std::make_shared<BodyPart>(BodyPart{"foot-right", {3.4825, 0.8892, 43.1269}});

	body->children.emplace_back(head);
	body->children.emplace_back(arm_left);
	body->children.emplace_back(arm_right);
	body->children.emplace_back(leg_left);
	body->children.emplace_back(leg_right);
	arm_left->children.emplace_back(hand_left);
	arm_right->children.emplace_back(hand_right);
	leg_left->children.emplace_back(foot_left);
	leg_right->children.emplace_back(foot_right);

	parts_.emplace(body->name, body);
	parts_.emplace(head->name, head);
	parts_.emplace(arm_left->name, arm_left);
	parts_.emplace(arm_right->name, arm_right);
	parts_.emplace(hand_left->name, hand_left);
	parts_.emplace(hand_right->name, hand_right);
	parts_.emplace(leg_left->name, leg_left);
	parts_.emplace(leg_right->name, leg_right);
	parts_.emplace(foot_left->name, foot_left);
	parts_.emplace(foot_right->name, foot_right);
}

std::shared_ptr<BodyPart> Body::find_part(std::string const& name) const {
	auto it = parts_.find(name);
	return parts_.end() == it ? nullptr : it->second;
}

void Body::apply_pose(Pose const& pose) {
	for (auto const& entry : pose.rotations) {
		if ("offset" == entry.first) {
			find_part("chest")->offset = entry.second;
			continue;
		}
		find_part(entry.first)->rotation = entry.second;
	}
}

std::ostream& Body::print(std::ostream& os) const {
	os << *find_part("chest");
	return os;
}

std::ostream& operator<<(std::ostream& os, Body const& body) {
	body.print(os);
	return os;
}
